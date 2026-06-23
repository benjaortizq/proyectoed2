#pragma once
#include <iostream>
#include <fstream>
#include <ctime>
#include <filesystem>
#include "algoritmos.cpp"


#define NOMBRE_ARCHIVO_RUTAS "RutasMinimas_"
#define NOMBRE_ARCHIVO_EXPANSION "ArbolDeExpansion_"
#define NOMBRE_ARCHIVO_ESTADISTICAS "Estadisticas_"
#define CARPETA_ARCHIVOS "files"

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







