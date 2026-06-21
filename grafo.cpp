
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


    // ===================== BUSQUEDA (auxiliares) =====================

    // Devuelve un puntero a la galaxia con ese id, o nullptr si no existe.
    Galaxia* buscarGalaxia (const string &id) {
        for (Galaxia &gal : this->galaxias) {
            if (gal.id == id) return &gal;
        }
        return nullptr;
    }

    // Devuelve un puntero a la ruta con ese id, o nullptr si no existe.
    Ruta* buscarRuta (const string &id) {
        for (Ruta &r : this->rutas) {
            if (r.id == id) return &r;
        }
        return nullptr;
    }


    // ===================== CRUD GALAXIA =====================

    // Agrega una galaxia. Falla (false) si ya existe una con el mismo id.
    bool agregarGalaxia (const Galaxia &nueva) {
        if (buscarGalaxia(nueva.id) != nullptr) {
            return false;   // id repetido -> no se permite
        }
        this->galaxias.push_back(nueva);
        this->totalNodos = (int)this->galaxias.size();
        return true;
    }

    // Edita los datos de una galaxia existente (identificada por id).
    bool editarGalaxia (const string &id, string codigo, string nombre, string tipo,
                        string descripcion, float x, float y, float z) {
        Galaxia* gal = buscarGalaxia(id);
        if (gal == nullptr) {
            return false;   // no existe
        }
        gal->Editar(codigo, nombre, tipo, descripcion, x, y, z);
        return true;
    }

    // Elimina una galaxia y, de paso, todas las rutas conectadas a ella
    // (no tendria sentido dejar rutas que apunten a una galaxia inexistente).
    bool eliminarGalaxia (const string &id) {
        if (buscarGalaxia(id) == nullptr) {
            return false;   // no existe
        }

        // Borrar rutas que tocan esa galaxia (recorriendo de atras hacia adelante
        // para que los indices no se corran al borrar).
        for (int i = (int)this->rutas.size() - 1; i >= 0; i--) {
            if (this->rutas[i].origen_id == id || this->rutas[i].destino_id == id) {
                this->rutas.erase(this->rutas.begin() + i);
            }
        }

        // Borrar la galaxia.
        for (int i = 0; i < (int)this->galaxias.size(); i++) {
            if (this->galaxias[i].id == id) {
                this->galaxias.erase(this->galaxias.begin() + i);
                break;
            }
        }

        this->totalNodos = (int)this->galaxias.size();
        this->totalAristas = (int)this->rutas.size();
        return true;
    }


    // ===================== CRUD RUTA =====================

    // Agrega una ruta. Falla (false) si el id ya existe o si el origen/destino
    // no corresponden a galaxias registradas (para asociar bien las entidades).
    bool agregarRuta (const Ruta &nueva) {
        if (buscarRuta(nueva.id) != nullptr) {
            return false;   // id de ruta repetido
        }
        if (buscarGalaxia(nueva.origen_id) == nullptr) {
            return false;   // el origen no existe como galaxia
        }
        if (buscarGalaxia(nueva.destino_id) == nullptr) {
            return false;   // el destino no existe como galaxia
        }
        this->rutas.push_back(nueva);
        this->totalAristas = (int)this->rutas.size();
        return true;
    }

    // Edita los datos de una ruta existente (identificada por id).
    bool editarRuta (const string &id, string origen_id, string destino_id, string tipo,
                     float costo, float tiempo_dias, bool activa) {
        Ruta* r = buscarRuta(id);
        if (r == nullptr) {
            return false;   // no existe
        }
        r->Editar(origen_id, destino_id, tipo, costo, tiempo_dias, activa);
        return true;
    }

    // Elimina la ruta con ese id.
    bool eliminarRuta (const string &id) {
        for (int i = 0; i < (int)this->rutas.size(); i++) {
            if (this->rutas[i].id == id) {
                this->rutas.erase(this->rutas.begin() + i);
                this->totalAristas = (int)this->rutas.size();
                return true;
            }
        }
        return false;   // no existe
    }

} ;

