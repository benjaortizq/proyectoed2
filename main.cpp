#include "menu.cpp"


// Si existe NOMBRE_GRAFO, carga TODO el estado (grafo principal + naves +
// historial) desde ese archivo en lugar de pedirlo a la API. Devuelve true si
// logro cargar desde el archivo; false si no existe o no se pudo leer (en ese
// caso cargardatos() usa la API).
bool cargarDesdeArchivo () {
    if (!fs::exists(NOMBRE_GRAFO)) {
        return false;
    }

    cout << CIAN_CLARO << "Archivo " << NOMBRE_GRAFO
         << " encontrado: cargando datos locales..." << RESET << endl;

    if (!parseAll(Principal, naves)) {
        cout << ROJO << "No se pudo leer " << NOMBRE_GRAFO
             << ", se usara la API." << RESET << endl;
        return false;
    }

    // Reconstruir el historial global "viajes" juntando el historial de cada nave.
    viajes.clear();
    for (const Nave &n : naves) {
        for (const Viaje &v : n.historial) {
            viajes.push_back(v);
        }
    }

    Principal.crearListaAdyacencia() ;
    k = kruskal(Principal) ;
    k.crearListaAdyacencia () ;

    cout << VERDE << "Datos cargados desde archivo: "
         << Principal.galaxias.size() << " galaxias, "
         << Principal.rutas.size() << " rutas, "
         << naves.size() << " naves." << RESET << endl;
    return true;
}


void cargardatos() {
    cout<<endl ;
    cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
    cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
    cout <<CIAN<<"-------------------II PROYECTO ESTRUCTURAS DE DATOS -------------------"<<RESET<<endl ;
    this_thread::sleep_for(chrono::milliseconds(500));
    cout << "~~~~~~~~~~~~~~ Benjamin Ortiz Quispe - 2025064599 ~~~~~~~~~~~~"<<endl ;
    this_thread::sleep_for(chrono::milliseconds(500));

    // Si hay un graph.txt guardado, se lee de ahi; si no, se baja todo de la API.
    if (!cargarDesdeArchivo()) {
        obtenerGrafoKurskal (Principal) ;


        rellenarDatosGrafo (Principal);

        obtenerNaves () ;
        obtenerViajes () ;
        asignarHistorialANaves(naves, viajes) ;   // copia a cada nave sus viajes

        Principal.crearListaAdyacencia() ;

        k = kruskal(Principal) ;
        k.crearListaAdyacencia () ;
    }

    archivosRUTASMINIMAS= contarArchivosConPrefijo(NOMBRE_ARCHIVO_RUTAS) ;
    archivosRUTASOPTIMIZADAS = contarArchivosConPrefijo(NOMBRE_ARCHIVO_EXPANSION);



    cout<<endl ;cout<<endl ;cout<<endl ;



}

int main () {
    SetTraceLogLevel(LOG_WARNING);
    cargardatos() ;
    
    

    menu() ;


}