#include <thread>
#include <chrono>
#include <future>

#include "gui.cpp"
#include "archivos.cpp"

#define RESET   "\033[0m"
#define ROJO    "\033[31m"
#define VERDE   "\033[32m"
#define AMARILLO "\033[33m"
#define CIAN    "\033[36m"
#define AZUL        "\033[34m"
#define MAJENTA     "\033[35m"



#define GRIS            "\033[90m"
#define ROJO_CLARO      "\033[91m"
#define VERDE_CLARO     "\033[92m"
#define AMARILLO_CLARO  "\033[93m"
#define AZUL_CLARO      "\033[94m"
#define MAGENTA_CLARO   "\033[95m"
#define CIAN_CLARO      "\033[96m"
#define BLANCO_CLARO    "\033[97m"

#define NEGRITA     "\033[1m"
#define TENUE       "\033[2m"
#define ITALICA     "\033[3m"
#define SUBRAYADO   "\033[4m"
#define PARPADEO    "\033[5m"
#define INVERTIDO   "\033[7m"




using namespace std ;

int leerOpcion () {
    int opcion;
    while (!(cin >> opcion)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << ROJO << "Entrada invalida. Ingrese un numero: " << RESET<<endl;
        this_thread::sleep_for(chrono::seconds(1));
        return -1; 
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');  // limpia el resto de la linea
    return opcion;
}



void obtenerGrafoKurskal (Grafo &g) {
    future<void> tarea = async(launch::async, [&g]() {
        GetGrafoURL(g, ENDPOINT);
    });

    int puntos = 0;
    while (tarea.wait_for(chrono::milliseconds(400)) != future_status::ready) {
        puntos = (puntos % 3) + 1;            // cicla 1, 2, 3, 1, 2, 3...
        string animacion(puntos, '.');        // "." , ".." , "..."
        cout << "\r" << AMARILLO << "Obteniendo grafo" << animacion << "   " << RESET << flush;
    }

    tarea.get();   

    cout << "\r" << VERDE << "Grafo obtenido!            " << RESET << endl;
}



void menuConsultas() { 
    int  o;

    do {
        cout  << CIAN << "========= Menu de Consultas ========="<<RESET<<endl ;
        o= leerOpcion() ;

    }
    while (o!=0) ;

}

void menuReportes() { 
    int o ;

    do {
        cout  << CIAN_CLARO << "========= Menu de Reportes ========="<<RESET<<endl ;
        o= leerOpcion() ;
    }
    while (o!=0) ;

}

void menuGrafo() { 
    int o ;
    do {
        cout  << AMARILLO << "========= Menu de Grafo ========="<<RESET<<endl ;
        o= leerOpcion() ;
    }
    while (o!=0) ;

}


void menu () {

    int o; 
    do {
        cout<<endl ;cout<<endl ;cout<<endl ;
        cout <<NEGRITA<<MAJENTA<<"-----------SISTEMA DE MODELADO DE GALAXIAS-----------"<<RESET<<endl ; 
        cout  << AZUL << "============ Menu principal ============"<<RESET<<endl ; 
        cout <<"1. Consultas "<<endl ; 
        cout <<"2. Reportes "<<endl ; 
        cout <<"3. Grafo "<<endl ; 
        cout <<"0. Salir "<<endl ;
        cout <<"Escriba una opcion : " ; 
        o= leerOpcion() ;
        if (o < 0 || o > 3) {
            cout << ROJO << "Solo opciones del 0 al 3.\n" << RESET;
            this_thread::sleep_for(chrono::seconds(1));
            continue;
        }

        switch (o) { 

            case 0 : {

                cout << MAJENTA<<"Hasta Luego!"<< RESET<<endl ;
                this_thread::sleep_for(chrono::seconds(1));
                break ;
            }
            case 1 : {
                menuConsultas() ;
                continue ;

            }
            case 2 : { 
                menuReportes () ;
                continue ;
            }

            case 3 : {
                menuGrafo () ;
                continue ;
            }
                
        

        }

    }while (o!=0) ; 

}