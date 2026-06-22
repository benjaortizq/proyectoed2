#include <thread>
#include <chrono>
#include <future>

#include "gui.cpp"


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
//!GENERACION DE ESTRUCTURAS AUXILIARES PARA ALMACENAMIETO DE RUTAS Y NAVES.

vector<Nave>  naves ;     // todas las naves y su estado
vector<Viaje> viajes ;    // todos los viajes (historial completo)

Grafo k ; 
Grafo Principal ; 

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

//!COSAS GRAFICAS

    


void obtenerNaves () {
    future<void> tarea = async(launch::async, []() {
        getNaves(naves) ;
    });

    int puntos = 0;
    while (tarea.wait_for(chrono::milliseconds(400)) != future_status::ready) {
        puntos = (puntos % 3) + 1;            // cicla 1, 2, 3, 1, 2, 3...
        string animacion(puntos, '.');        // "." , ".." , "..."
        cout << "\r" << CIAN_CLARO << "Obteniendo Naves" << animacion << "   " << RESET << flush;
    }

    tarea.get();   

    cout << VERDE << "Naves cargadas: " << naves.size()<< RESET << endl;
};

void obtenerViajes () {
    future<void> tarea = async(launch::async, []() {
        getHistorial(viajes) ;
    });

    int puntos = 0;
    while (tarea.wait_for(chrono::milliseconds(400)) != future_status::ready) {
        puntos = (puntos % 3) + 1;            // cicla 1, 2, 3, 1, 2, 3...
        string animacion(puntos, '.');        // "." , ".." , "..."
        cout << "\r" << VERDE_CLARO << "Obteniendo Historial" << animacion << "   " << RESET << flush;
    }

    tarea.get();   

    cout << VERDE <<"Viajes cargados: " << viajes.size() << RESET << endl ;
}


void obtenerGrafoKurskal (Grafo &g) {
    int puntos = 0;
    auto inicio = chrono::steady_clock::now() ;
    const auto TIMEOUT = chrono::seconds (60) ;

    // Reintentar hasta lograr datos o hasta agotar el timeout global.
    while (true) {
        // Un intento de descarga en segundo plano.
        future<void> tarea = async(launch::async, [&g]() {
            GetGrafoURL(g, ENDPOINT);
        });

        // Esperar a que ESTE intento termine, animando los puntos.
        while (tarea.wait_for(chrono::milliseconds(400)) != future_status::ready) {
            puntos = (puntos % 3) + 1;            // cicla 1, 2, 3, 1, 2, 3...
            string animacion(puntos, '.');        // "." , ".." , "..."
            cout << "\r" << AMARILLO << "Conectando con EndPoint " << animacion << "   " << RESET << flush;

            if (chrono::steady_clock::now() - inicio >= TIMEOUT) {
                cout << "\r" << ROJO << "Error: Tiempo de espera agotado (60s). Verifique la conexion." << RESET << endl;
                exit(1);
            }
        }
        tarea.get();

        // Exito: GetGrafoURL trajo aristas -> salimos.
        if (!g.rutas.empty()) {
            break;
        }

        // Fallo (sin internet o servidor caido): GetGrafoURL volvio vacio.
        // Reintentar si todavia queda tiempo.
        if (chrono::steady_clock::now() - inicio >= TIMEOUT) {
            cout << "\r" << ROJO << "Error: Tiempo de espera agotado (60s). Verifique la conexion." << RESET << endl;
            exit(1);
        }
        puntos = (puntos % 3) + 1;            // cicla 1, 2, 3, 1, 2, 3...
        string animacion(puntos, '.');        // "." , ".." , "..."
        cout << "\r" << ROJO_CLARO << "Sin respuesta del servidor, reintentando" <<animacion<<"          "<< RESET << flush;
        this_thread::sleep_for(chrono::seconds(2));   // pausa antes del siguiente intento
    }

    cout << "\r" << VERDE << "Grafo inicial obtenido!            " << RESET << endl;
}

void rellenarDatosGrafo (Grafo &g) {
    future<void> tarea = async(launch::async, [&g]() {
        fillGalaxiaData(g);
    });

    int puntos = 0;
    while (tarea.wait_for(chrono::milliseconds(400)) != future_status::ready) {
        puntos = (puntos % 3) + 1;            // cicla 1, 2, 3, 1, 2, 3...
        string animacion(puntos, '.');        // "." , ".." , "..."
        cout << "\r" << AMARILLO << "Obteniendo datos de Galaxias" << animacion << "   " << RESET << flush;
    }

    tarea.get();   

    cout << "\r" << VERDE << "Datos de Galaxias obtenidas!            " << RESET << endl;
}



//!MENUS-------------------------------------------------


void rutamascortaentreGalaxias() {
    int g1 ;
    int g2 ;
    do {
        cout  << CIAN << "--------------Lista de todas las Galaxias--------------"<<RESET<<endl ;
        cout<<endl ;
        Principal.printGalaxias() ;
        cout<<endl ;cout<<endl ;
        cout<<"Escriba el numero de la galaxia origen (0 para volver) : " ;

        g1=leerOpcion () ;
        if(g1==0) { 
            break;
        }
        string id1 = "galaxia-" + to_string(g1) ;
        Galaxia t1 = Principal.getGalaxia (id1) ;
        cout<<endl ;

        if (t1.id == "") {                            // si getGalaxia no la encontro
            cout<< ROJO<<"Entrada invalida. La galaxia no existe"<<RESET<<endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue;
        }

        this_thread::sleep_for(chrono::milliseconds(100));

        cout<<"Escriba el numero de la galaxia destino (0 para volver ) : " ;

        g2=leerOpcion () ;
        if (g2 == 0) { 
            break;
        }
        string id2 = "galaxia-" + to_string(g2) ;
        Galaxia t2 = Principal.getGalaxia (id2) ;
        cout<<endl ;

        if (t2.id == "") {
            cout<< ROJO<<"Entrada invalida. La galaxia no existe"<<RESET<<endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue;
        }

        // Calcular el camino de menor costo y mostrarlo.
        vector<Ruta> camino = dijkstra (Principal , id1 , id2) ;
        cout<<endl ;
        imprimirCamino (Principal , id1 , camino) ;
        cout<<endl ;

        // Si hubo camino, abrir la ventana con esa ruta resaltada en rojo.
        if (!camino.empty()) {
            dibujarGrafo (Principal , camino) ;
        }
        break;

    }while (true) ;

}

void MenuConsultasRutasdesdeGalaxia () { 
    int o ;
    bool a = true ;
    do  {

        cout  << CIAN << "--------------Lista de todas las Galaxias--------------"<<RESET<<endl ;
        cout<<endl ;
        Principal.printGalaxias() ;
        cout<<endl ;cout<<endl ;
        cout<<"Escriba el numero de galaxia a consultar (o 0 para volver): " ;
        o=leerOpcion () ;
        if (o==0) { 
            break ;
        }
        cout<<endl ;
        string id = "galaxia-" + to_string(o) ;
        Galaxia t = Principal.getGalaxia (id) ;
        if (t.nombre == "") { 
            cout<< ROJO<<"Entrada invalida. La galaxia no existe"<<RESET<<endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue;
        }
        this_thread::sleep_for(chrono::milliseconds(100));

        Principal.imprimirAdyacencia(t) ;
        break;

    }while (a) ;
}

void arbolConexiones(Grafo&g ) {    
    k = kruskal(g) ; 
    GraficarARBOL (k) ;
} ;


void historialdeViajes() { 

} ; 

void menuConsultas() { 
    int  o;

    do {
        cout<<endl ;cout<<endl ;cout<<endl ;
        cout  << CIAN << "========= Menu de Consultas ========="<<RESET<<endl ;
        cout <<"1. Rutas desde una Galaxia "<<endl ; 
        cout <<"2. Ruta de menor costo entre dos Galaxias "<<endl ; 
        cout <<"3. Arbol de conexiones  "<<endl ; 
        cout <<"4. Historial de viajes por nave   "<<endl ; 
        cout <<"0. Volver "<<endl ;cout<<endl ;
        cout <<"Escriba una opcion : " ; 

        o= leerOpcion() ;
        if (o < 0 || o > 4) {
            cout << ROJO << "Solo opciones del 0 al 4.\n" << RESET;
            this_thread::sleep_for(chrono::seconds(1));
            continue;
        }

        switch (o) {

            case 0 : {
                break ;
            }


            case 1 : { 
                MenuConsultasRutasdesdeGalaxia() ;
                continue;

            }

            case 2 : {
                rutamascortaentreGalaxias() ;
                continue ;
                }

            case 3: {
                arbolConexiones (Principal) ;
                continue ;
            }

            case 4 : { 
                historialdeViajes() ; 
                continue;

            }
        }
    }
    while (o!=0) ;

};











void menuReportes() { 
    int o ;

    do {
        cout<<endl ;cout<<endl ;cout<<endl ;
        cout  << VERDE_CLARO << "========= Menu de Reportes ========="<<RESET<<endl ;
        cout <<"1. Generar Archivo con las rutas mas cortas generadas "<<endl ; 
        cout <<"2. Generar Archivo de Arbol de Expansion "<<endl ; 
        cout <<"3. Generar Informe de estadisticas"<<endl ; 
        cout <<"0. Volver "<<endl ;cout<<endl ;
        cout <<"Escriba una opcion : " ; 
        o= leerOpcion() ;
        if (o < 0 || o > 3) {
            cout << ROJO << "Solo opciones del 0 al 3.\n" << RESET;
            this_thread::sleep_for(chrono::seconds(1));
            continue;
        }
    }
    while (o!=0) ;

}













void menuGrafo() { 
    int o ;
    do {
        cout<<endl ;cout<<endl ;cout<<endl ;
        cout  << AMARILLO << "========= Menu de Grafo ========="<<RESET<<endl ;
        cout <<"1. Opciones de Galaxias "<<endl ; 
        cout <<"2. Opciones de Rutas"<<endl ; 
        cout <<"3. Opciones de Naves"<<endl ;
        cout <<"0. Volver "<<endl ;cout<<endl ;
        cout <<"Escriba una opcion : " ; 
        o= leerOpcion() ;

        if (o < 0 || o > 3) {
            cout << ROJO << "Solo opciones del 0 al 3.\n" << RESET;
            this_thread::sleep_for(chrono::seconds(1));
            continue;
        }
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
        cout <<"0. Salir "<<endl ;cout<<endl ;
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