#include <curl/curl.h>
#include <regex>
#include <string>
#include <vector>
#include "grafo.cpp"

using namespace std ; 

#define ENDPOINT "https://galaxias-mock-api.onrender.com/grafo/kruskal"//ENDPOINT DE DONDE SE SACA EL GRAFO
#define GALAXIA_ENDPOINT "https://galaxias-mock-api.onrender.com/galaxia/"
#define RUTA_ENDPOINT "https://galaxias-mock-api.onrender.com/rutas/"

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




void getGalaxiaData(Galaxia& g) {
	string url = string(GALAXIA_ENDPOINT) + g.id;
	string response = fetchUrl(url);

	if (response.empty()) return;

	smatch match;

	if (regex_search(response, match, regex("\"codigo\"\\s*:\\s*\"([^\"]+)\"")))
		g.codigo = match[1].str();

	if (regex_search(response, match, regex("\"nombre\"\\s*:\\s*\"([^\"]+)\"")))
		g.nombre = match[1].str();

	if (regex_search(response, match, regex("\"tipo\"\\s*:\\s*\"([^\"]+)\"")))
		g.tipo = match[1].str();

	if (regex_search(response, match, regex("\"x\"\\s*:\\s*(-?[0-9]+(?:\\.[0-9]+)?)")))
		g.x = stof(match[1].str());

	if (regex_search(response, match, regex("\"y\"\\s*:\\s*(-?[0-9]+(?:\\.[0-9]+)?)")))
		g.y = stof(match[1].str());

	if (regex_search(response, match, regex("\"z\"\\s*:\\s*(-?[0-9]+(?:\\.[0-9]+)?)")))
		g.z = stof(match[1].str());

	if (regex_search(response, match, regex("\"descripcion\"\\s*:\\s*\"([^\"]+)\"")))
		g.descripcion = match[1].str();
}

void getRutaData(Ruta&R) { 
	if (R.id.empty()) return;

	string url = string(RUTA_ENDPOINT) + R.id;
	string response = fetchUrl(url);

	if (response.empty()) return;

	smatch match;

	if (regex_search(response, match, regex("\"id\"\\s*:\\s*\"([^\"]+)\"")))
		R.id = match[1].str();

	if (regex_search(response, match, regex("\"origen_id\"\\s*:\\s*\"([^\"]+)\"")))
		R.origen_id = match[1].str();

	if (regex_search(response, match, regex("\"destino_id\"\\s*:\\s*\"([^\"]+)\"")))
		R.destino_id = match[1].str();

	if (regex_search(response, match, regex("\"tipo\"\\s*:\\s*\"([^\"]+)\"")))
		R.tipo = match[1].str();

	if (regex_search(response, match, regex("\"costo\"\\s*:\\s*(-?[0-9]+(?:\\.[0-9]+)?)")))
		R.costo = stof(match[1].str());

	if (regex_search(response, match, regex("\"tiempo_dias\"\\s*:\\s*(-?[0-9]+(?:\\.[0-9]+)?)")))
		R.tiempo_dias = stof(match[1].str());

	if (regex_search(response, match, regex("\"activa\"\\s*:\\s*(true|false)")))
		R.activa = match[1].str() == "true";
}