#pragma once
#include <vector>
#include "nave.cpp"
#include "ruta.cpp"

using namespace std ;


// Una entrada de la lista de adyacencia: una galaxia y todas las rutas que la tocan.
struct Adyacencia {
    Galaxia galaxia ;        // el nodo
    vector<Ruta> rutas ;     // rutas conectadas a esa galaxia
} ;


struct Grafo {
    int totalNodos;
    int totalAristas;
    bool esDirigido;
    string version;
    vector<Galaxia> galaxias;
    vector<Ruta> rutas;
    vector<Adyacencia> listaAdyacencia;   // se llena con crearListaAdyacencia()

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


    // ===================== LISTA DE ADYACENCIA =====================

    // Construye la lista de adyacencia: por cada galaxia, todas las rutas que la
    // tocan. Como el grafo es NO dirigido, una ruta A-B aparece tanto en la
    // entrada de A como en la de B. Llamar DESPUES de cargar el grafo del endpoint.
    void crearListaAdyacencia () {
        this->listaAdyacencia.clear();

        for (const Galaxia &gal : this->galaxias) {
            Adyacencia entrada;
            entrada.galaxia = gal;

            for (const Ruta &r : this->rutas) {
                if (r.origen_id == gal.id || r.destino_id == gal.id) {
                    entrada.rutas.push_back(r);
                }
            }

            this->listaAdyacencia.push_back(entrada);
        }
    }

    // Devuelve todas las rutas posibles desde una galaxia (segun la lista de
    // adyacencia ya construida). Vector vacio si la galaxia no existe.
    vector<Ruta> rutasDesdeGalaxia (const string &id) {
        for (const Adyacencia &entrada : this->listaAdyacencia) {
            if (entrada.galaxia.id == id) {
                return entrada.rutas;
            }
        }
        return vector<Ruta>();
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
        crearListaAdyacencia() ;
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
                crearListaAdyacencia();
                return true;
            }
        }
        return false;   // no existe
    }

} ;

