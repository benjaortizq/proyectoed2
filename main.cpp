#include "menu.cpp"


void cargardatos() { 
    cout<<endl ;
    cout<<endl ;
    cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
    cout <<CIAN<<"-------------------II PROYECTO ESTRUCTURAS DE DATOS -------------------"<<RESET<<endl ; 
    this_thread::sleep_for(chrono::milliseconds(500));
    cout << "~~~~~~~~~~~~~~ Benjamin Ortiz Quispe - 2025064599 ~~~~~~~~~~~~"<<endl ;
    this_thread::sleep_for(chrono::milliseconds(500));
    obtenerGrafoKurskal (Principal) ;
    rellenarDatosGrafo (Principal);
    obtenerNaves () ;
    obtenerViajes () ;
    asignarHistorialANaves(naves, viajes) ;   // copia a cada nave sus viajes
    Principal.crearListaAdyacencia() ;
    archivosRUTASMINIMAS= contarArchivosConPrefijo(NOMBRE_ARCHIVO_RUTAS) ;


}

int main () {
    SetTraceLogLevel(LOG_WARNING);
    cargardatos() ;
    menu() ;

}