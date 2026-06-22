#pragma once
#include <iostream>
#include <fstream>
#include <ctime>
#include <filesystem>
#include "algoritmos.cpp"


#define NOMBRE_ARCHIVO_RUTAS "RutasMinimas_"
#define CARPETA_ARCHIVOS "files"

using namespace std ;
namespace fs = std::filesystem ;


vector<vector<Ruta>> rutasGeneradas ;
int archivosRUTASMINIMAS ;
int archivosCONECTIVIDAD; 
int archivosRUTASOPTIMIZADAS;


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

    archivo.close();
    return true;
}







