
// estos son los nodos de la API o algo asi xd 

#include <iostream>
using namespace std  ; 
struct Galaxia  { 
    string id  ; 
    string codigo; 
    string nombre ; 
    string tipo ; 
    float x; // mucho despiche harlo short , el parseado es un despiche 
    float y; 
    float z ; 
    string descripcion  ;  // esta vaina es biel larga

    public : 

    Galaxia ()  {  // iniciador vacio , tiene que tener inicializador vacio a params igual 
        this->id = "" ; 
        this->codigo= "" ; 
        this->nombre = "" ; 
        this->tipo = "" ; 
        this->descripcion = "" ;
        this->x = 0.0 ; 
        this->y = 0.0 ; 
        this->z = 0.0 ; 
    }

    Galaxia (string id , string codigo , string nombre , string tipo , string descripcion , float x , float y , float z) { 
        this->id = id ; 
        this->codigo= codigo; 
        this->nombre = nombre ; 
        this->tipo = tipo ; 
        this->descripcion = descripcion ;
        this->x = x ; 
        this->y = y ; 
        this->z = z ; 
    } ; 

    // Reemplaza todos los datos de la galaxia (menos el id, que es su clave).
    void Editar (string codigo , string nombre , string tipo , string descripcion , float x , float y , float z) {
        this->codigo = codigo ;
        this->nombre = nombre ;
        this->tipo = tipo ;
        this->descripcion = descripcion ;
        this->x = x ;
        this->y = y ;
        this->z = z ;
    }

    void print () const {
        cout << "Galaxia{" << endl;
        cout << "  id: " << this->id << endl;
        cout << "  codigo: " << this->codigo << endl;
        cout << "  nombre: " << this->nombre << endl;
        cout << "  tipo: " << this->tipo << endl;
        cout << "  descripcion: " << this->descripcion << endl;
        cout << "  x: " << this->x << endl;
        cout << "  y: " << this->y << endl;
        cout << "  z: " << this->z << endl;
        cout << "}" << endl;
    }
} ;
