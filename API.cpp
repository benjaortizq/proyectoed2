#include <curl/curl.h>
#include <regex>
#include <string>
#include <vector>
#include <map>
#include "grafo.cpp"
#include "nave.cpp"

using namespace std ;

#define ENDPOINT "https://galaxias-mock-api.onrender.com/grafo/kruskal"//ENDPOINT DE DONDE SE SACA EL GRAFO
#define GALAXIA_ENDPOINT "https://galaxias-mock-api.onrender.com/galaxia/"
#define RUTA_ENDPOINT "https://galaxias-mock-api.onrender.com/rutas/"
#define GRAFO_ENDPOINT "https://galaxias-mock-api.onrender.com/grafo/"//grafo completo con todos los datos de los nodos
#define HISTORIAL_ENDPOINT "https://galaxias-mock-api.onrender.com/Historial/"
#define NAVES_ENDPOINT "https://galaxias-mock-api.onrender.com/naves"


static size_t writeCallback (void *contents, size_t size, size_t nmemb, void *userp) {
	string *response = static_cast<string*>(userp);
	response->append(static_cast<char*>(contents), size * nmemb);
	return size * nmemb;
}

static string fetchUrl (const string &url) {
	CURL *curl = curl_easy_init();
	if (!curl) {
		return "";
	}

	string response;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");

	CURLcode result = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if (result != CURLE_OK) {
		return "";
	}

	return response;
}

static string getArrayBlock (const string &json, const string &key) {
	size_t keyPosition = json.find("\"" + key + "\"");
	if (keyPosition == string::npos) {
		return "";
	}

	size_t arrayStart = json.find('[', keyPosition);
	if (arrayStart == string::npos) {
		return "";
	}

	int depth = 0;
	for (size_t index = arrayStart; index < json.size(); ++index) {
		if (json[index] == '[') {
			depth++;
		}
		else if (json[index] == ']') {
			depth--;
			if (depth == 0) {
				return json.substr(arrayStart + 1, index - arrayStart - 1);
			}
		}
	}

	return "";
}

static vector<string> splitObjects (const string &arrayContent) {
	vector<string> objects;
	size_t start = string::npos;
	int depth = 0;

	for (size_t index = 0; index < arrayContent.size(); ++index) {
		if (arrayContent[index] == '{') {
			if (depth == 0) {
				start = index;
			}
			depth++;
		}
		else if (arrayContent[index] == '}') {
			depth--;
			if (depth == 0 && start != string::npos) {
				objects.push_back(arrayContent.substr(start, index - start + 1));
				start = string::npos;
			}
		}
	}

	return objects;
}

// Devuelve el contenido del PRIMER arreglo [...] que aparezca en el json.
// Sirve tanto si la respuesta es una lista directa "[ {...}, {...} ]" como si
// viene envuelta en un objeto "{ \"algo\": [ {...} ] }".
static string getFirstArray (const string &json) {
	size_t arrayStart = json.find('[');
	if (arrayStart == string::npos) {
		return "";
	}

	int depth = 0;
	for (size_t index = arrayStart; index < json.size(); ++index) {
		if (json[index] == '[') {
			depth++;
		}
		else if (json[index] == ']') {
			depth--;
			if (depth == 0) {
				return json.substr(arrayStart + 1, index - arrayStart - 1);
			}
		}
	}

	return "";
}


void GetGrafoURL (Grafo&g , string URL) { // usable solo con /kuyrskal / grafo
	string response = fetchUrl(URL);

	g = Grafo();

	if (response.empty()) {
		return;
	}

	smatch match;
	if (regex_search(response, match, regex(R"("total_nodos"\s*:\s*(\d+))"))) {
		g.totalNodos = stoi(match[1].str());
	}

	if (regex_search(response, match, regex(R"("total_aristas"\s*:\s*(\d+))"))) {
		g.totalAristas = stoi(match[1].str());
	}

	string aristasArray = getArrayBlock(response, "aristas");
	vector<string> objects = splitObjects(aristasArray);

	for (const string &object : objects) {
		smatch routeMatch;
		regex routeRegex("\\{\\\"id\\\"\\s*:\\s*\\\"([^\\\"]+)\\\"\\s*,\\s*\\\"origen_id\\\"\\s*:\\s*\\\"([^\\\"]+)\\\"\\s*,\\s*\\\"destino_id\\\"\\s*:\\s*\\\"([^\\\"]+)\\\"\\s*,\\s*\\\"tipo\\\"\\s*:\\s*\\\"([^\\\"]+)\\\"\\s*,\\s*\\\"costo\\\"\\s*:\\s*([0-9]+(?:\\.[0-9]+)?)\\s*,\\s*\\\"tiempo_dias\\\"\\s*:\\s*([0-9]+(?:\\.[0-9]+)?)\\s*,\\s*\\\"activa\\\"\\s*:\\s*(true|false)\\s*\\}");

		if (regex_search(object, routeMatch, routeRegex)) {
			Ruta ruta(
				routeMatch[1].str(),
				routeMatch[2].str(),
				routeMatch[3].str(),
				routeMatch[4].str(),
				stof(routeMatch[5].str()),
				stof(routeMatch[6].str()),
				routeMatch[7].str() == "true"
			);

			g.rutas.push_back(ruta);

			bool origenExiste = false;
			bool destinoExiste = false;

			for (const Galaxia &galaxia : g.galaxias) {
				if (galaxia.id == ruta.origen_id) {
					origenExiste = true;
				}
				if (galaxia.id == ruta.destino_id) {
					destinoExiste = true;
				}
			}

			if (!origenExiste) {
				Galaxia origen;
				origen.id = ruta.origen_id;
				g.galaxias.push_back(origen);
			}

			if (!destinoExiste) {
				Galaxia destino;
				destino.id = ruta.destino_id;
				g.galaxias.push_back(destino);
			}
		}
	}
}




// Pide el grafo COMPLETO (/grafo/) que trae todos los datos de cada nodo, arma
// un diccionario id -> Galaxia con todas las galaxias, y luego rellena los datos
// (codigo, nombre, tipo, x, y, z, descripcion) SOLO de las galaxias que ya
// existen en el grafo "g" que se recibe (p.ej. el que devolvio /grafo/kruskal,
// que solo trae el id de cada nodo).
void fillGalaxiaData(Grafo& g) {
	string response = fetchUrl(GRAFO_ENDPOINT);

	if (response.empty()) return;

	// 1. Parsear TODOS los nodos del grafo completo en un diccionario id -> Galaxia.
	map<string, Galaxia> galaxiasPorId;

	string nodosArray = getArrayBlock(response, "nodos");
	vector<string> objects = splitObjects(nodosArray);

	for (const string& object : objects) {
		Galaxia galaxia;
		smatch match;

		if (regex_search(object, match, regex("\"id\"\\s*:\\s*\"([^\"]+)\"")))
			galaxia.id = match[1].str();

		if (regex_search(object, match, regex("\"codigo\"\\s*:\\s*\"([^\"]+)\"")))
			galaxia.codigo = match[1].str();

		if (regex_search(object, match, regex("\"nombre\"\\s*:\\s*\"([^\"]+)\"")))
			galaxia.nombre = match[1].str();

		if (regex_search(object, match, regex("\"tipo\"\\s*:\\s*\"([^\"]+)\"")))
			galaxia.tipo = match[1].str();

		if (regex_search(object, match, regex("\"x\"\\s*:\\s*(-?[0-9]+(?:\\.[0-9]+)?)")))
			galaxia.x = stof(match[1].str());

		if (regex_search(object, match, regex("\"y\"\\s*:\\s*(-?[0-9]+(?:\\.[0-9]+)?)")))
			galaxia.y = stof(match[1].str());

		if (regex_search(object, match, regex("\"z\"\\s*:\\s*(-?[0-9]+(?:\\.[0-9]+)?)")))
			galaxia.z = stof(match[1].str());

		if (regex_search(object, match, regex("\"descripcion\"\\s*:\\s*\"([^\"]+)\"")))
			galaxia.descripcion = match[1].str();

		if (!galaxia.id.empty())
			galaxiasPorId[galaxia.id] = galaxia;
	}


	for (Galaxia& galaxia : g.galaxias) {
		map<string, Galaxia>::iterator it = galaxiasPorId.find(galaxia.id);
		if (it != galaxiasPorId.end()) {
			galaxia = it->second;
		}
	}

}


// Trae todos los viajes (historial) del endpoint y los guarda en "viajes".
void getHistorial (vector<Viaje>& viajes) {
	viajes.clear();

	string response = fetchUrl(HISTORIAL_ENDPOINT);
	if (response.empty()) return;

	string arrayContent = getFirstArray(response);
	vector<string> objects = splitObjects(arrayContent);

	for (const string& object : objects) {
		Viaje v;
		smatch m;

		if (regex_search(object, m, regex("\"id\"\\s*:\\s*\"([^\"]*)\"")))
			v.id = m[1].str();
		if (regex_search(object, m, regex("\"nave_id\"\\s*:\\s*\"([^\"]*)\"")))
			v.nave_id = m[1].str();
		if (regex_search(object, m, regex("\"ruta_id\"\\s*:\\s*\"([^\"]*)\"")))
			v.ruta_id = m[1].str();
		if (regex_search(object, m, regex("\"fecha\"\\s*:\\s*\"([^\"]*)\"")))
			v.fecha = m[1].str();
		if (regex_search(object, m, regex("\"costo_real\"\\s*:\\s*(-?[0-9]+(?:\\.[0-9]+)?)")))
			v.costo_real = stof(m[1].str());
		if (regex_search(object, m, regex("\"exitoso\"\\s*:\\s*(true|false)")))
			v.exitoso = (m[1].str() == "true");
		if (regex_search(object, m, regex("\"nave_nombre\"\\s*:\\s*\"([^\"]*)\"")))
			v.nave_nombre = m[1].str();
		if (regex_search(object, m, regex("\"origen_nombre\"\\s*:\\s*\"([^\"]*)\"")))
			v.origen_nombre = m[1].str();
		if (regex_search(object, m, regex("\"destino_nombre\"\\s*:\\s*\"([^\"]*)\"")))
			v.destino_nombre = m[1].str();

		if (!v.id.empty()) viajes.push_back(v);
	}
}


// Trae todas las naves del endpoint y las guarda en "naves".
void getNaves (vector<Nave>& naves) {
	naves.clear();

	string response = fetchUrl(NAVES_ENDPOINT);
	if (response.empty()) return;

	string arrayContent = getFirstArray(response);
	vector<string> objects = splitObjects(arrayContent);

	for (const string& object : objects) {
		Nave n;
		smatch m;

		if (regex_search(object, m, regex("\"id\"\\s*:\\s*\"([^\"]*)\"")))
			n.id = m[1].str();
		if (regex_search(object, m, regex("\"codigo\"\\s*:\\s*\"([^\"]*)\"")))
			n.codigo = m[1].str();
		if (regex_search(object, m, regex("\"nombre\"\\s*:\\s*\"([^\"]*)\"")))
			n.nombre = m[1].str();
		if (regex_search(object, m, regex("\"capacidad\"\\s*:\\s*([0-9]+)")))
			n.capacidad = stoi(m[1].str());
		// el campo de velocidad podria llamarse de varias formas en la API:
		if (regex_search(object, m, regex("\"velocidad_maxima\"\\s*:\\s*([0-9]+)")) ||
		    regex_search(object, m, regex("\"velocidad_max\"\\s*:\\s*([0-9]+)")) ||
		    regex_search(object, m, regex("\"velocidad\"\\s*:\\s*([0-9]+)")))
			n.velocidad_Max = stoi(m[1].str());
		if (regex_search(object, m, regex("\"activa\"\\s*:\\s*(true|false)")))
			n.activa = (m[1].str() == "true");

		if (!n.id.empty()) naves.push_back(n);
	}
}