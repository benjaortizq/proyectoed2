#pragma once
#include <vector>
#include "galaxia.cpp"
#include "historial.cpp"

using namespace std  ;

struct Nave {
    string id ;
    string codigo;
    string nombre ;
    int capacidad ;
    int velocidad_Max ;
    bool activa;
    vector<Viaje> historial ;   // copias de los viajes realizados por esta nave

    public :

    Nave () {   // constructor vacio
        this->id = "" ;
        this->codigo = "" ;
        this->nombre = "" ;
        this->capacidad = 0 ;
        this->velocidad_Max = 0 ;
        this->activa = false ;
    }

    Nave (string id , string codigo , string nombre , int capacidad , int velocidad_Max , bool activa) {
        this->id = id ;
        this->codigo = codigo ;
        this->nombre = nombre ;
        this->capacidad = capacidad ;
        this->velocidad_Max = velocidad_Max ;
        this->activa = activa ;
    }

    // Reemplaza todos los datos de la nave (menos el id, que es su clave).
    void Editar (string codigo , string nombre , int capacidad , int velocidad_Max , bool activa) {
        this->codigo = codigo ;
        this->nombre = nombre ;
        this->capacidad = capacidad ;
        this->velocidad_Max = velocidad_Max ;
        this->activa = activa ;
    }

    void print () const {
        cout << "Nave{" << endl;
        cout << "  id: " << this->id << endl;
        cout << "  codigo: " << this->codigo << endl;
        cout << "  nombre: " << this->nombre << endl;
        cout << "  capacidad: " << this->capacidad << endl;
        cout << "  velocidad_Max: " << this->velocidad_Max << endl;
        cout << "  activa: " << (this->activa ? "true" : "false") << endl;
        cout << "  viajes: " << this->historial.size() << endl;
        cout << "}" << endl;
    }

} ;