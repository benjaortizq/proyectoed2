#include "menu.cpp"
int main () {
    cout<<endl ;
    cout<<endl ;
    cout<<endl ;
    cout <<CIAN<<"-------------------II PROYECTO ESTRUCTURAS DE DATOS -------------------"<<RESET<<endl ; 
    this_thread::sleep_for(chrono::milliseconds(500));
    cout << "~~~~~~~~~~~~~~ Benjamin Ortiz Quispe - 2025064599 ~~~~~~~~~~~~"<<endl ;
    this_thread::sleep_for(chrono::milliseconds(500));

    Grafo Principal ; 

    obtenerGrafoKurskal (Principal) ;

    menu() ; 

}