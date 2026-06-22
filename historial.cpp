#pragma once
// Un registro de viaje (lo que devuelve la API por cada viaje del historial).
#include <iostream>
#include <string>
using namespace std ;

struct Viaje {
    string id ;
    string nave_id ;
    string ruta_id ;
    string fecha ;
    float  costo_real ;
    bool   exitoso ;
    string nave_nombre ;
    string origen_nombre ;
    string destino_nombre ;

    public :

    Viaje () {   // constructor vacio
        this->id = "" ;
        this->nave_id = "" ;
        this->ruta_id = "" ;
        this->fecha = "" ;
        this->costo_real = 0.0 ;
        this->exitoso = false ;
        this->nave_nombre = "" ;
        this->origen_nombre = "" ;
        this->destino_nombre = "" ;
    }

    Viaje (string id , string nave_id , string ruta_id , string fecha , float costo_real ,
           bool exitoso , string nave_nombre , string origen_nombre , string destino_nombre) {
        this->id = id ;
        this->nave_id = nave_id ;
        this->ruta_id = ruta_id ;
        this->fecha = fecha ;
        this->costo_real = costo_real ;
        this->exitoso = exitoso ;
        this->nave_nombre = nave_nombre ;
        this->origen_nombre = origen_nombre ;
        this->destino_nombre = destino_nombre ;
    }

    // Reemplaza todos los datos del viaje (menos el id, que es su clave).
    void Editar (string nave_id , string ruta_id , string fecha , float costo_real ,
                 bool exitoso , string nave_nombre , string origen_nombre , string destino_nombre) {
        this->nave_id = nave_id ;
        this->ruta_id = ruta_id ;
        this->fecha = fecha ;
        this->costo_real = costo_real ;
        this->exitoso = exitoso ;
        this->nave_nombre = nave_nombre ;
        this->origen_nombre = origen_nombre ;
        this->destino_nombre = destino_nombre ;
    }

    void print () const {
        cout << "Viaje{" << endl;
        cout << "  id: " << this->id << endl;
        cout << "  nave_id: " << this->nave_id << endl;
        cout << "  ruta_id: " << this->ruta_id << endl;
        cout << "  fecha: " << this->fecha << endl;
        cout << "  costo_real: " << this->costo_real << endl;
        cout << "  exitoso: " << (this->exitoso ? "true" : "false") << endl;
        cout << "  nave_nombre: " << this->nave_nombre << endl;
        cout << "  origen_nombre: " << this->origen_nombre << endl;
        cout << "  destino_nombre: " << this->destino_nombre << endl;
        cout << "}" << endl;
    }

} ;
