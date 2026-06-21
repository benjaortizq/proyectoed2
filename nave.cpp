#pragma once
#include "galaxia.cpp"

using namespace std  ;

struct Nave {
    string id ;
    string codigo;
    string nombre ;
    int capacidad ;
    int velocidad_Max ;
    bool activa;

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

} ;