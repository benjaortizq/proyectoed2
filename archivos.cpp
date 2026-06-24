#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <ctime>
#include <filesystem>
#include "algoritmos.cpp"


#define NOMBRE_ARCHIVO_RUTAS "RutasMinimas_"
#define NOMBRE_ARCHIVO_EXPANSION "ArbolDeExpansion_"
#define NOMBRE_ARCHIVO_ESTADISTICAS "Estadisticas_"
#define CARPETA_ARCHIVOS "files"
#define NOMBRE_GRAFO "graph.txt"

using namespace std ;
namespace fs = std::filesystem ;


// Caminos (resultados de dijkstra) generados en la sesion:
//  - rutasGeneradas: calculados sobre el grafo PRINCIPAL (red original).
//  - rutasGeneradasKurskal: calculados sobre el ARBOL de Kruskal (red optimizada).
// Se llenan EN PARALELO: rutasGeneradas[i] y rutasGeneradasKurskal[i] son el
// mismo par origen-destino, uno en cada red.
vector<vector<Ruta>> rutasGeneradas ;
vector<vector<Ruta>> rutasGeneradasKurskal ;
int archivosRUTASMINIMAS ;
int archivosCONECTIVIDAD; 
int archivosRUTASOPTIMIZADAS;
int archivoESTADISTICAS ;


// Devuelve la fecha de hoy como "AnyoMesDia" (ej: 20260622).
string fechaHoy () {
    time_t t = time(nullptr);
    tm* lt = localtime(&t);
    char buffer[16];
    strftime(buffer, sizeof(buffer), "%Y%m%d", lt);
    return string(buffer);
}

// Cuenta cuantos archivos en /files empiezan con un prefijo dado.
int contarArchivosConPrefijo (const string &prefijo) {
    int cantidad = 0;
    if (fs::exists(CARPETA_ARCHIVOS) && fs::is_directory(CARPETA_ARCHIVOS)) {
        for (const auto &entrada : fs::directory_iterator(CARPETA_ARCHIVOS)) {
            string nombre = entrada.path().filename().string();
            if (nombre.rfind(prefijo, 0) == 0) {   // rfind(..,0)==0 -> empieza con prefijo
                cantidad++;
            }
        }
    }
    return cantidad ;
}


// Genera un archivo en /files con las rutas (caminos) generados en esta sesion,
// guardados en la variable global rutasGeneradas. El nombre es:
//   RutasMinimas_<AnyoMesDia>_<cantidad de archivos RutasMinimas_ ya existentes>.txt
bool GenerarArchivoRutasMinimas () {
    string nombre = string(NOMBRE_ARCHIVO_RUTAS) + fechaHoy() + "_" + to_string(archivosRUTASMINIMAS) + ".txt";
    string ruta = string(CARPETA_ARCHIVOS) + "/" + nombre;

    ofstream archivo(ruta);
    if (!archivo.is_open()) {
        return false;
    }

    archivo << "==================================================\n";
    archivo << " REPORTE DE RUTAS MINIMAS GENERADAS\n";
    archivo << " Fecha: " << fechaHoy() << "\n";
    archivo << " Rutas generadas en esta sesion: " << rutasGeneradas.size() << "\n";
    archivo << "==================================================\n\n";

    if (rutasGeneradas.empty()) {
        archivo << "(No se generaron rutas en esta sesion)\n";
    }

    int numero = 1;
    archivo << "Rutas calculadas en el Grafo principal : \n\n";
    for (const vector<Ruta> &camino : rutasGeneradas) {
        archivo << "--- Ruta #" << numero << " ---\n";

        if (camino.empty()) {
            archivo << "  (sin conexion entre las galaxias)\n\n";
            numero++;
            continue;
        }

        float costoTotal = 0.0f;
        float tiempoTotal = 0.0f;
        for (const Ruta &r : camino) {
            archivo << "  " << r.origen_id << " -> " << r.destino_id
                    << "   [" << r.id << ", costo " << r.costo
                    << ", " << r.tiempo_dias << " dias]\n";
            costoTotal  += r.costo;
            tiempoTotal += r.tiempo_dias;
        }
        archivo << "  Costo total: " << costoTotal
                << "  |  Tiempo total: " << tiempoTotal << " dias\n\n";
        numero++;
    }
        numero = 1 ; // se resetea numero
        archivo << "Rutas calculadas en el Grafo Optimizado : \n\n";
        for (const vector<Ruta> &camino : rutasGeneradasKurskal) {
        archivo << "--- Ruta #" << numero << " ---\n";

        if (camino.empty()) {
            archivo << "  (sin conexion entre las galaxias)\n\n";
            numero++;
            continue;
        }

        float costoTotal = 0.0f;
        float tiempoTotal = 0.0f;
        for (const Ruta &r : camino) {
            archivo << "  " << r.origen_id << " -> " << r.destino_id
                    << "   [" << r.id << ", costo " << r.costo
                    << ", " << r.tiempo_dias << " dias]\n";
            costoTotal  += r.costo;
            tiempoTotal += r.tiempo_dias;
        }
        archivo << "  Costo total: " << costoTotal
                << "  |  Tiempo total: " << tiempoTotal << " dias\n\n";
        numero++;
    }

    archivo.close();
    return true;
}


// Genera un archivo en /files con el contenido de un grafo (pensado para el
// arbol de expansion que devuelve kruskal). Imprime las galaxias y las rutas en
// formato tabla, con todos sus datos. El nombre es:
//   ArbolDeExpansion_<AnyoMesDia>_<cantidad de archivos ArbolDeExpansion_ ya existentes>.txt
bool generarArchivoRutasExpansion (Grafo &g) {
    
    string nombre = string(NOMBRE_ARCHIVO_EXPANSION) + fechaHoy() + "_" + to_string(archivosRUTASOPTIMIZADAS) + ".txt";
    string ruta = string(CARPETA_ARCHIVOS) + "/" + nombre;

    ofstream archivo(ruta);
    if (!archivo.is_open()) {
        return false;
    }

    streambuf* coutOriginal = cout.rdbuf();
    cout.rdbuf(archivo.rdbuf());

    cout << "==================================================\n";
    cout << " ARBOL DE EXPANSION (RUTA GALACTICA OPTIMIZADA)\n";
    cout << " Fecha: " << fechaHoy() << "\n";
    cout << " Galaxias: " << g.galaxias.size() << "   |   Rutas: " << g.rutas.size() << "\n";
    cout << "==================================================\n\n";

    cout << "GALAXIAS:\n";
    g.printGalaxias();

    cout << "\nRUTAS DEL ARBOL:\n";
    g.printRutas();
    cout.rdbuf(coutOriginal);

    archivo.close();
    return true;
}


// Suma el costo total de un camino (lista de rutas).
float costoCamino (const vector<Ruta> &camino) {
    float total = 0.0f;
    for (const Ruta &r : camino) total += r.costo;
    return total;
}

// Devuelve los dos extremos (origen, destino) de un camino. En un camino simple,
// los extremos son las galaxias que aparecen una sola vez entre todas las rutas.
pair<string, string> extremosCamino (const vector<Ruta> &camino) {
    if (camino.empty()) return { "", "" };
    if (camino.size() == 1) return { camino[0].origen_id, camino[0].destino_id };

    map<string, int> apariciones;
    for (const Ruta &r : camino) {
        apariciones[r.origen_id]++;
        apariciones[r.destino_id]++;
    }

    vector<string> extremos;
    for (const pair<const string, int> &p : apariciones) {
        if (p.second == 1) extremos.push_back(p.first);
    }

    if (extremos.size() == 2) return { extremos[0], extremos[1] };
    return { camino.front().origen_id, camino.back().destino_id };   // fallback
}


// Genera un archivo de estadisticas con tres secciones:
//   1. Cantidad de rutas calculadas (Dijkstra) en el principal y en Kruskal.
//   2. Galaxias con mayor conectividad (en el principal y en Kruskal).
//   3. Comparacion ruta por ruta: original vs optimizada, con veredicto.
bool generarArchivoEstadisticas (Grafo &principal, Grafo &kruskalGrafo) {
    string nombre = string(NOMBRE_ARCHIVO_ESTADISTICAS) + fechaHoy() + "_" + to_string(archivoESTADISTICAS) + ".txt";
    string ruta = string(CARPETA_ARCHIVOS) + "/" + nombre;

    ofstream archivo(ruta);
    if (!archivo.is_open()) {
        return false;
    }

    // Reusar las tablas del grafo (escriben con cout) redirigiendo cout al archivo.
    streambuf* coutOriginal = cout.rdbuf();
    cout.rdbuf(archivo.rdbuf());

    cout << "==================================================\n";
    cout << " INFORME DE ESTADISTICAS\n";
    cout << " Fecha: " << fechaHoy() << "\n";
    cout << "==================================================\n\n";

    // -------- 1. Cantidad de rutas calculadas --------
    cout << "1) CANTIDAD DE RUTAS CALCULADAS (Dijkstra)\n";
    cout << "--------------------------------------------------\n";
    cout << "   En el grafo principal : " << rutasGeneradas.size() << "\n";
    cout << "   En el arbol de Kruskal: " << rutasGeneradasKurskal.size() << "\n\n";

    cout << "   Desglose (grafo principal):\n";
    for (size_t i = 0; i < rutasGeneradas.size(); i++) {
        pair<string, string> ext = extremosCamino(rutasGeneradas[i]);
        cout << "     #" << (i + 1) << " " << ext.first << " - " << ext.second
             << "  (" << rutasGeneradas[i].size() << " saltos, costo "
             << costoCamino(rutasGeneradas[i]) << ")\n";
    }
    cout << "\n   Desglose (arbol de Kruskal):\n";
    for (size_t i = 0; i < rutasGeneradasKurskal.size(); i++) {
        pair<string, string> ext = extremosCamino(rutasGeneradasKurskal[i]);
        cout << "     #" << (i + 1) << " " << ext.first << " - " << ext.second
             << "  (" << rutasGeneradasKurskal[i].size() << " saltos, costo "
             << costoCamino(rutasGeneradasKurskal[i]) << ")\n";
    }
    cout << "\n";

    // -------- 2. Galaxias con mayor conectividad --------
    cout << "2) GALAXIAS CON MAYOR CONECTIVIDAD\n";
    cout << "--------------------------------------------------\n";
    cout << "[ Grafo principal ]\n";
    principal.printPorConectividad();
    cout << "[ Arbol de Kruskal ]\n";
    kruskalGrafo.printPorConectividad();
    cout << "\n";

    // -------- 3. Comparacion original vs optimizada --------
    cout << "3) COMPARACION: RUTAS ORIGINALES vs OPTIMIZADAS\n";
    cout << "--------------------------------------------------\n";
    size_t n = min(rutasGeneradas.size(), rutasGeneradasKurskal.size());
    for (size_t i = 0; i < n; i++) {
        pair<string, string> ext = extremosCamino(rutasGeneradas[i]);
        float costoOriginal   = costoCamino(rutasGeneradas[i]);
        float costoOptimizada = costoCamino(rutasGeneradasKurskal[i]);

        cout << "Ruta #" << (i + 1) << "  (" << ext.first << " - " << ext.second << ")\n";
        cout << "   Original  (principal): costo " << costoOriginal
             << ", " << rutasGeneradas[i].size() << " saltos\n";
        cout << "   Optimizada (Kruskal) : costo " << costoOptimizada
             << ", " << rutasGeneradasKurskal[i].size() << " saltos\n";

        cout << "   Veredicto: ";
        if (costoOriginal < costoOptimizada) {
            cout << "la ORIGINAL es mas eficiente (ahorra " << (costoOptimizada - costoOriginal) << ")\n";
        } else if (costoOptimizada < costoOriginal) {
            cout << "la OPTIMIZADA es mas eficiente (ahorra " << (costoOriginal - costoOptimizada) << ")\n";
        } else {
            cout << "ambas tienen el mismo costo\n";
        }
        cout << "\n";
    }

    cout.rdbuf(coutOriginal);   // restaurar cout SIEMPRE
    archivo.close();
    return true;
}




// ===================== PERSISTENCIA JSON (graph.txt) =====================
//
// serializeAll() vuelca TODO el estado (grafo principal + naves con su historial)
// a NOMBRE_GRAFO en formato JSON. parseAll() hace la operacion inversa: lee ese
// mismo archivo y reconstruye el grafo y las naves. Asi el programa puede arrancar
// sin internet, leyendo los datos guardados en lugar de pedirlos a la API.
//
// Para parsear se reutilizan getArrayBlock() y splitObjects() de API.cpp (mismo
// archivo de compilacion) mas unos pequenos auxiliares de lectura de campos.


// Escapa un string para meterlo entre comillas en JSON (comillas, backslash y
// saltos de linea). Sin esto un nombre o descripcion con comillas romperia el JSON.
static string escaparJson (const string &s) {
    string out;
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:   out += c;      break;
        }
    }
    return out;
}

// Lee el valor de un campo string "clave":"valor" dentro de obj, deshaciendo los
// escapes (\" \\ \n \r \t). Devuelve "" si la clave no esta.
static string leerCampoStr (const string &obj, const string &clave) {
    string needle = "\"" + clave + "\"";
    size_t p = obj.find(needle);
    if (p == string::npos) return "";

    p = obj.find(':', p + needle.size());
    if (p == string::npos) return "";
    p++;                                                 // pasar el ':'
    while (p < obj.size() && (obj[p] == ' ' || obj[p] == '\t')) p++;
    if (p >= obj.size() || obj[p] != '"') return "";     // no es un string
    p++;                                                 // entrar al contenido

    string out;
    while (p < obj.size() && obj[p] != '"') {
        if (obj[p] == '\\' && p + 1 < obj.size()) {
            char n = obj[p + 1];
            switch (n) {
                case '"':  out += '"';  break;
                case '\\': out += '\\'; break;
                case 'n':  out += '\n'; break;
                case 'r':  out += '\r'; break;
                case 't':  out += '\t'; break;
                default:   out += n;    break;
            }
            p += 2;
        } else {
            out += obj[p];
            p++;
        }
    }
    return out;
}

// Lee el valor de un campo numerico "clave": numero. Devuelve 0 si no esta.
static float leerCampoFloat (const string &obj, const string &clave) {
    smatch m;
    regex re("\"" + clave + "\"\\s*:\\s*(-?[0-9]+(?:\\.[0-9]+)?)");
    if (regex_search(obj, m, re)) return stof(m[1].str());
    return 0.0f;
}

static int leerCampoInt (const string &obj, const string &clave) {
    smatch m;
    regex re("\"" + clave + "\"\\s*:\\s*(-?[0-9]+)");
    if (regex_search(obj, m, re)) return stoi(m[1].str());
    return 0;
}

// Lee un campo booleano "clave": true|false. Devuelve false si no esta.
static bool leerCampoBool (const string &obj, const string &clave) {
    smatch m;
    regex re("\"" + clave + "\"\\s*:\\s*(true|false)");
    if (regex_search(obj, m, re)) return m[1].str() == "true";
    return false;
}


// Serializa el grafo "g" y las naves (con su historial) a NOMBRE_GRAFO en JSON.
// Devuelve false si no pudo abrir el archivo para escribir.
bool serializeAll (Grafo &g, vector<Nave> &naves) {
    ofstream archivo(NOMBRE_GRAFO);
    if (!archivo.is_open()) {
        return false;
    }

    archivo << "{\n";

    // -------- grafo principal --------
    archivo << "  \"grafo\": {\n";
    archivo << "    \"total_nodos\": "   << g.totalNodos   << ",\n";
    archivo << "    \"total_aristas\": " << g.totalAristas << ",\n";
    archivo << "    \"es_dirigido\": "   << (g.esDirigido ? "true" : "false") << ",\n";
    archivo << "    \"version\": \""     << escaparJson(g.version) << "\",\n";

    // galaxias (nodos)
    archivo << "    \"galaxias\": [\n";
    for (size_t i = 0; i < g.galaxias.size(); i++) {
        const Galaxia &gal = g.galaxias[i];
        archivo << "      {"
                << "\"id\": \""          << escaparJson(gal.id)          << "\", "
                << "\"codigo\": \""      << escaparJson(gal.codigo)      << "\", "
                << "\"nombre\": \""      << escaparJson(gal.nombre)      << "\", "
                << "\"tipo\": \""        << escaparJson(gal.tipo)        << "\", "
                << "\"descripcion\": \"" << escaparJson(gal.descripcion) << "\", "
                << "\"x\": " << gal.x << ", "
                << "\"y\": " << gal.y << ", "
                << "\"z\": " << gal.z
                << "}";
        archivo << (i + 1 < g.galaxias.size() ? ",\n" : "\n");
    }
    archivo << "    ],\n";

    // rutas (aristas)
    archivo << "    \"rutas\": [\n";
    for (size_t i = 0; i < g.rutas.size(); i++) {
        const Ruta &r = g.rutas[i];
        archivo << "      {"
                << "\"id\": \""         << escaparJson(r.id)         << "\", "
                << "\"origen_id\": \""  << escaparJson(r.origen_id)  << "\", "
                << "\"destino_id\": \"" << escaparJson(r.destino_id) << "\", "
                << "\"tipo\": \""       << escaparJson(r.tipo)       << "\", "
                << "\"costo\": "        << r.costo       << ", "
                << "\"tiempo_dias\": "  << r.tiempo_dias << ", "
                << "\"activa\": "       << (r.activa ? "true" : "false")
                << "}";
        archivo << (i + 1 < g.rutas.size() ? ",\n" : "\n");
    }
    archivo << "    ]\n";
    archivo << "  },\n";

    // -------- naves con su historial --------
    archivo << "  \"naves\": [\n";
    for (size_t i = 0; i < naves.size(); i++) {
        const Nave &n = naves[i];
        archivo << "    {\n";
        archivo << "      \"id\": \""    << escaparJson(n.id)     << "\", "
                << "\"codigo\": \""      << escaparJson(n.codigo) << "\", "
                << "\"nombre\": \""      << escaparJson(n.nombre) << "\", "
                << "\"capacidad\": "     << n.capacidad     << ", "
                << "\"velocidad_max\": " << n.velocidad_Max << ", "
                << "\"activa\": "        << (n.activa ? "true" : "false") << ",\n";

        archivo << "      \"historial\": [\n";
        for (size_t j = 0; j < n.historial.size(); j++) {
            const Viaje &v = n.historial[j];
            archivo << "        {"
                    << "\"id\": \""             << escaparJson(v.id)             << "\", "
                    << "\"nave_id\": \""        << escaparJson(v.nave_id)        << "\", "
                    << "\"ruta_id\": \""        << escaparJson(v.ruta_id)        << "\", "
                    << "\"fecha\": \""          << escaparJson(v.fecha)          << "\", "
                    << "\"costo_real\": "       << v.costo_real << ", "
                    << "\"exitoso\": "          << (v.exitoso ? "true" : "false") << ", "
                    << "\"nave_nombre\": \""    << escaparJson(v.nave_nombre)    << "\", "
                    << "\"origen_nombre\": \""  << escaparJson(v.origen_nombre)  << "\", "
                    << "\"destino_nombre\": \"" << escaparJson(v.destino_nombre) << "\""
                    << "}";
            archivo << (j + 1 < n.historial.size() ? ",\n" : "\n");
        }
        archivo << "      ]\n";
        archivo << "    }";
        archivo << (i + 1 < naves.size() ? ",\n" : "\n");
    }
    archivo << "  ]\n";

    archivo << "}\n";
    archivo.close();
    return true;
}


// Lee NOMBRE_GRAFO (JSON producido por serializeAll) y reconstruye el grafo "g"
// y la lista de naves "naves" (cada una con su historial). Devuelve false si el
// archivo no existe, esta vacio o no se pudo abrir.
bool parseAll (Grafo &g, vector<Nave> &naves) {
    ifstream archivo(NOMBRE_GRAFO);
    if (!archivo.is_open()) {
        return false;
    }

    stringstream buffer;
    buffer << archivo.rdbuf();
    string contenido = buffer.str();
    archivo.close();
    if (contenido.empty()) {
        return false;
    }

    g = Grafo();
    naves.clear();

    // -------- meta del grafo --------
    g.esDirigido = leerCampoBool(contenido, "es_dirigido");
    g.version    = leerCampoStr(contenido, "version");

    // -------- galaxias (nodos) --------
    string galaxiasArray = getArrayBlock(contenido, "galaxias");
    vector<string> galaxiaObjs = splitObjects(galaxiasArray);
    for (const string &obj : galaxiaObjs) {
        Galaxia gal;
        gal.id          = leerCampoStr(obj, "id");
        gal.codigo      = leerCampoStr(obj, "codigo");
        gal.nombre      = leerCampoStr(obj, "nombre");
        gal.tipo        = leerCampoStr(obj, "tipo");
        gal.descripcion = leerCampoStr(obj, "descripcion");
        gal.x           = leerCampoFloat(obj, "x");
        gal.y           = leerCampoFloat(obj, "y");
        gal.z           = leerCampoFloat(obj, "z");
        if (!gal.id.empty()) g.galaxias.push_back(gal);
    }

    // -------- rutas (aristas) --------
    string rutasArray = getArrayBlock(contenido, "rutas");
    vector<string> rutaObjs = splitObjects(rutasArray);
    for (const string &obj : rutaObjs) {
        Ruta r;
        r.id          = leerCampoStr(obj, "id");
        r.origen_id   = leerCampoStr(obj, "origen_id");
        r.destino_id  = leerCampoStr(obj, "destino_id");
        r.tipo        = leerCampoStr(obj, "tipo");
        r.costo       = leerCampoFloat(obj, "costo");
        r.tiempo_dias = leerCampoFloat(obj, "tiempo_dias");
        r.activa      = leerCampoBool(obj, "activa");
        if (!r.id.empty()) g.rutas.push_back(r);
    }

    g.totalNodos   = (int)g.galaxias.size();
    g.totalAristas = (int)g.rutas.size();

    // -------- naves con su historial --------
    string navesArray = getArrayBlock(contenido, "naves");
    vector<string> naveObjs = splitObjects(navesArray);
    for (const string &obj : naveObjs) {
        Nave n;
        n.id            = leerCampoStr(obj, "id");
        n.codigo        = leerCampoStr(obj, "codigo");
        n.nombre        = leerCampoStr(obj, "nombre");
        n.capacidad     = leerCampoInt(obj, "capacidad");
        n.velocidad_Max = leerCampoInt(obj, "velocidad_max");
        n.activa        = leerCampoBool(obj, "activa");

        // El historial es un arreglo anidado dentro de la nave.
        string histArray = getArrayBlock(obj, "historial");
        vector<string> viajeObjs = splitObjects(histArray);
        for (const string &vobj : viajeObjs) {
            Viaje v;
            v.id             = leerCampoStr(vobj, "id");
            v.nave_id        = leerCampoStr(vobj, "nave_id");
            v.ruta_id        = leerCampoStr(vobj, "ruta_id");
            v.fecha          = leerCampoStr(vobj, "fecha");
            v.costo_real     = leerCampoFloat(vobj, "costo_real");
            v.exitoso        = leerCampoBool(vobj, "exitoso");
            v.nave_nombre    = leerCampoStr(vobj, "nave_nombre");
            v.origen_nombre  = leerCampoStr(vobj, "origen_nombre");
            v.destino_nombre = leerCampoStr(vobj, "destino_nombre");
            if (!v.id.empty()) n.historial.push_back(v);
        }

        if (!n.id.empty()) naves.push_back(n);
    }

    return true;
}




