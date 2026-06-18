
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

    Grafo () { 
        this->totalAristas = this->totalNodos = 0 ; 
        this->esDirigido = false ; 
        this->version = "" ; 
    }

    void print () {
        cout << "Grafo{" << endl;
        cout << "  totalNodos: " << this->totalNodos << endl;
        cout << "  totalAristas: " << this->totalAristas << endl;
        cout << "  esDirigido: " << (this->esDirigido ? "true" : "false") << endl;
        cout << "  version: " << this->version << endl;

        cout << "  galaxias: [" << endl;
        for (const Galaxia &galaxia : this->galaxias) {
            galaxia.print();
        }
        cout << "  ]" << endl;

        cout << "  rutas: [" << endl;
        for (const Ruta &ruta : this->rutas) {
            ruta.print();
        }
        cout << "  ]" << endl;
        cout << "}" << endl;
    }

} ;

