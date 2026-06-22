#include "menu.cpp"
int main () {
    cout<<endl ;
    cout<<endl ;
    cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
    cout <<CIAN<<"-------------------II PROYECTO ESTRUCTURAS DE DATOS -------------------"<<RESET<<endl ; 
    this_thread::sleep_for(chrono::milliseconds(500));
    cout << "~~~~~~~~~~~~~~ Benjamin Ortiz Quispe - 2025064599 ~~~~~~~~~~~~"<<endl ;
    this_thread::sleep_for(chrono::milliseconds(500));

    //!PRECARGA DE DATOS 

    Grafo Principal ; 
    obtenerGrafoKurskal (Principal) ;
    rellenarDatosGrafo (Principal);

    //!GENERACION DE ESTRUCTURAS AUXILIARES PARA ALMACENAMIETO DE RUTAS Y NAVES.

    vector<Nave>  naves ;     // todas las naves y su estado
    vector<Viaje> viajes ;    // todos los viajes (historial completo)

    getNaves(naves) ;
    getHistorial(viajes) ;
    asignarHistorialANaves(naves, viajes) ;   // copia a cada nave sus viajes

    cout << VERDE << "Naves cargadas: " << naves.size()
         << "  | Viajes cargados: " << viajes.size() << RESET << endl ;

    menu() ;

}