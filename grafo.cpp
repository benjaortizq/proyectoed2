
#include <vector> 
#include "galaxia.cpp"
#include "ruta.cpp"

using namespace std ;


struct Grafo { 
    int totalNodos;
    int totalAristas;
    bool esDirigido;
    string version;
    vector<Galaxia> galaxias;
    vector<Ruta> rutas;
} ;

