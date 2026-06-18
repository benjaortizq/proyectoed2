// esto es litealmente las Aristas de lo que la API me da xd  
#include <string>
using namespace std  ; 

struct Ruta  { 
    string id  ; 
    string origen_id  ; 
    string destino_id ; 
    string tipo ; 
    float costo ; 
    float tiempo_dias; 
    bool activa ;
    public :
    Ruta() {
        this->id = "";
        this->origen_id = "";
        this->destino_id = "";
        this->tipo = "";
        this->costo = 0.0;
        this->tiempo_dias = 0.0;
        this->activa = false;
    }

    Ruta(string id, string origen_id, string destino_id, string tipo, float costo, float tiempo_dias, bool activa) {
        this->id = id;
        this->origen_id = origen_id;
        this->destino_id = destino_id;
        this->tipo = tipo;
        this->costo = costo;
        this->tiempo_dias = tiempo_dias;
        this->activa = activa;
    }

    void print () const {
        cout << "Ruta{" << endl;
        cout << "  id: " << this->id << endl;
        cout << "  origen_id: " << this->origen_id << endl;
        cout << "  destino_id: " << this->destino_id << endl;
        cout << "  tipo: " << this->tipo << endl;
        cout << "  costo: " << this->costo << endl;
        cout << "  tiempo_dias: " << this->tiempo_dias << endl;
        cout << "  activa: " << (this->activa ? "true" : "false") << endl;
        cout << "}" << endl;
    }

};