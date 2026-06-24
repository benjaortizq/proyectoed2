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
        cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
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

        // Calcular el camino de menor costo en CADA red (una sola vez).
        vector<Ruta> camino  = dijkstra (Principal , id1 , id2) ;   // red original
        vector<Ruta> caminoK = dijkstra (k , id1 , id2) ;           // red optimizada (Kruskal)

        // Guardar en paralelo para los reportes (mismo par en ambos vectores).
        rutasGeneradas.push_back( camino ) ;
        rutasGeneradasKurskal.push_back( caminoK ) ;

        cout<<endl ;
        cout<<"Ruta en el grafo Principal: " <<endl ;
        imprimirCamino (Principal , id1 , camino) ;
        cout<<endl ;
        cout<<"Ruta en el grafo  'Optimizado' : " <<endl ;
        imprimirCamino (k , id1 , caminoK) ;        // <- ahora el camino de Kruskal
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
    do  {
        cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;

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
        Principal.imprimirAdyacencia(t) ;
        this_thread::sleep_for(chrono::milliseconds(1500));
        break;

    }while (true) ;
}

void arbolConexiones() {    
    k = kruskal(Principal) ; 
    GraficarARBOL (k) ;
} ;


void historialdeViajes() { 
    int o ; 

    do {
        cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
        cout  << CIAN << "----------------Lista de todas las Naves---------------"<<RESET<<endl ;
        cout<<endl ;
        imprimirNaves (naves);
        cout<<endl ;cout<<endl ;
        cout<<"Escriba el numero de nave a consultar (o 0 para volver): " ;
        o=leerOpcion () ;
        if (o==0) { 
            break ;
        }
        cout<<endl ;
        string id = "nave-" + to_string(o) ;
        Nave t = buscarNave (naves,id) ; 
        if (t.nombre == "") { 
            cout<< ROJO<<"Entrada invalida. La nave no existe"<<RESET<<endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue;
        }
        
        imprimirViajesDeNave(t) ;
        this_thread::sleep_for(chrono::milliseconds(1500));
        break ; 


    }

    while (true) ;


} ; 

void menuConsultas() { 
    int  o;
    do {
        cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
        cout  << CIAN << "========= Menu de Consultas ========="<<RESET<<endl ;
        cout<<endl ;
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
                arbolConexiones () ;
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

//!REPORTEES--------------------------------------------------------------------------

void menuRutasCortas() { 

    if (rutasGeneradas.empty() ) { 
        cout<<ROJO_CLARO<<"NO se han generado rutas minimas en esta sesion "<<RESET<<endl;
        this_thread::sleep_for(chrono::seconds(1));
        return ;
    }
    if(GenerarArchivoRutasMinimas()){ 
        cout<<VERDE<<"Archivo generado correctamente"<<RESET<<endl ;
        this_thread::sleep_for(chrono::seconds(1));
        archivosRUTASMINIMAS= contarArchivosConPrefijo(NOMBRE_ARCHIVO_RUTAS) ;
        return;
    } ; 
    cout<<ROJO<<"Error en la creacion del archivo ;"<<RESET<<endl ;
    this_thread::sleep_for(chrono::seconds(1));
    return ;
}

void menuRutasExpansion() { 
    k = kruskal(Principal) ; 
    
    if(generarArchivoRutasExpansion(k)){ 
        cout<<VERDE<<"Archivo generado correctamente"<<RESET<<endl ;
        this_thread::sleep_for(chrono::seconds(1));
        archivosRUTASOPTIMIZADAS= contarArchivosConPrefijo(NOMBRE_ARCHIVO_RUTAS) ;
        return ;
    } ; 
    cout<<ROJO<<"Error en la creacion del archivo ;"<<RESET<<endl ;
    this_thread::sleep_for(chrono::seconds(1));
    return ;

}

void menuEstadisticas () { 
    if (generarArchivoEstadisticas (Principal,k)) { 
        cout<<VERDE<<"Archivo generado correctamente"<<RESET<<endl ;
        this_thread::sleep_for(chrono::seconds(1));
        archivoESTADISTICAS = contarArchivosConPrefijo (NOMBRE_ARCHIVO_ESTADISTICAS) ;
        return ;
    }
    cout<<ROJO<<"Error en la creacion del archivo ;"<<RESET<<endl ;
    this_thread::sleep_for(chrono::seconds(1));
    return ;
}








void menuReportes() { 
    int o ;

    do {
        cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
        cout  << VERDE_CLARO << "========= Menu de Reportes ========="<<RESET<<endl ;
        cout<<endl ;
        cout <<"1. Generar Archivo con las rutas mas cortas generadas "<<endl ; 
        cout <<"2. Generar Archivo de Arbol de Expansion "<<endl ; 
        cout <<"3. Generar Archivo de Estadisticas "<<endl ; 
        cout <<"0. Volver "<<endl ;cout<<endl ;
        cout <<"Escriba una opcion : " ; 
        o= leerOpcion() ;
        if (o < 0 || o > 3) {
            cout << ROJO << "Solo opciones del 0 al 3.\n" << RESET;
            this_thread::sleep_for(chrono::seconds(1));
            continue;
        }

        switch (o) { 

            case 1 : {
                menuRutasCortas() ; 
                continue;
            }

            case 2 : {
                menuRutasExpansion() ; 
                continue;


            }
            case 3 : {
                menuEstadisticas () ;
                continue;


            }
            

        }
    }
    while (o!=0) ;

}







//!MENU CRUD ESTRUCTURAS -----------------------------------------------------


//*--------------------------------------------------------------//--------------------------------------------------------------
void smCrearGalaxia () {
    // Variables de la nueva galaxia.
    string codigo ;
    string nombre ;
    string tipo ;
    string descripcion ;
    float  x = 0.0f ;
    float  y = 0.0f ;
    float  z = 0.0f ;

    cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
    cout << NEGRITA << MAGENTA_CLARO << "============ Crear Nueva Galaxia ============" << RESET << endl ;
    cout << endl ;

    // 1) Codigo (texto, no puede quedar vacio y no puede repetirse).
    while (true) {
        cout << "Escriba el codigo de la galaxia: " ;
        getline(cin, codigo) ;
        if (codigo.empty()) {
            cout << ROJO << "El codigo no puede quedar vacio." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        if (Principal.buscarGalaxiaPorCodigo(codigo) != nullptr) {
            cout << ROJO << "Ya existe una galaxia con ese codigo." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        break ;
    }

    // 2) Nombre (texto, no puede quedar vacio).
    while (true) {
        cout << "Escriba el nombre de la galaxia: " ;
        getline(cin, nombre) ;
        if (nombre.empty()) {
            cout << ROJO << "El nombre no puede quedar vacio." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        break ;
    }

    // 3) Tipo (texto, no puede quedar vacio).
    while (true) {
        cout << "Escriba el tipo de la galaxia (ej: espiral, eliptica): " ;
        getline(cin, tipo) ;
        if (tipo.empty()) {
            cout << ROJO << "El tipo no puede quedar vacio." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        break ;
    }

    // 4) Descripcion (texto, puede quedar vacio).
    cout << "Escriba la descripcion de la galaxia (opcional): " ;
    getline(cin, descripcion) ;

    // 5) Coordenada X (cualquier numero, puede ser negativo).
    while (true) {
        cout << "Escriba la coordenada X: " ;
        if (cin >> x) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
            break ;
        }
        cin.clear() ;
        cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
        cout << ROJO << "Valor invalido. Ingrese un numero." << RESET << endl ;
        this_thread::sleep_for(chrono::seconds(1));
    }

    // 6) Coordenada Y.
    while (true) {
        cout << "Escriba la coordenada Y: " ;
        if (cin >> y) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
            break ;
        }
        cin.clear() ;
        cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
        cout << ROJO << "Valor invalido. Ingrese un numero." << RESET << endl ;
        this_thread::sleep_for(chrono::seconds(1));
    }

    // 7) Coordenada Z.
    while (true) {
        cout << "Escriba la coordenada Z: " ;
        if (cin >> z) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
            break ;
        }
        cin.clear() ;
        cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
        cout << ROJO << "Valor invalido. Ingrese un numero." << RESET << endl ;
        this_thread::sleep_for(chrono::seconds(1));
    }

    // Generar un id unico ("galaxia-N" sin chocar con los existentes).
    int n = (int)Principal.galaxias.size() + 1 ;
    string nuevoId = "galaxia-" + to_string(n) ;
    while (Principal.buscarGalaxia(nuevoId) != nullptr) {
        n++ ;
        nuevoId = "galaxia-" + to_string(n) ;
    }

    // Crear el objeto Galaxia y agregarlo al grafo principal.
    Galaxia nueva(nuevoId, codigo, nombre, tipo, descripcion, x, y, z) ;
    if (!Principal.agregarGalaxia(nueva)) {
        cout << ROJO << "No se pudo crear la galaxia (id repetido)." << RESET << endl ;
        this_thread::sleep_for(chrono::seconds(2));
        return ;
    }

    // Reconstruir la adyacencia y recalcular el arbol de expansion (Kruskal).
    Principal.crearListaAdyacencia() ;
    k = kruskal(Principal) ;
    k.crearListaAdyacencia() ;

    cout << endl ;
    cout << VERDE << "Galaxia '" << nuevoId << "' creada correctamente:" << RESET << endl ;
    nueva.print() ;
    this_thread::sleep_for(chrono::seconds(2));
}


void smEditarGalaxia() {
    Galaxia* g = nullptr ;

    // Seleccionar la galaxia a editar: se listan y se pide el numero -> "galaxia-N".
    while (true) {
        cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
        cout << ITALICA << MAGENTA_CLARO << "---------- Editar Galaxia ----------" << RESET << endl ;
        cout << endl ;
        Principal.printGalaxias() ;
        cout << endl ;
        cout << "Escriba el numero de la galaxia a editar (0 para volver): " ;

        int o = leerOpcion() ;
        if (o == 0) {
            return ;
        }
        if (o < 0) continue ;   // leerOpcion ya aviso de la entrada invalida
        string id = "galaxia-" + to_string(o) ;
        g = Principal.buscarGalaxia(id) ;
        if (g == nullptr) {                       // puntero vacio -> no existe
            cout << ROJO << "La galaxia no existe." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        break ;
    }

    // Mostrar la galaxia encontrada.
    cout << endl ;
    cout << VERDE << "Editando la siguiente galaxia:" << RESET << endl ;
    g->print() ;
    cout << endl ;
    cout << AMARILLO << "Para cada campo escriba el nuevo valor. Enter VACIO mantiene el valor actual." << RESET << endl ;
    cout << endl ;

    // Variables para los nuevos valores (parten del valor actual).
    string nuevoCodigo = g->codigo ;
    string nuevoNombre = g->nombre ;
    string nuevoTipo   = g->tipo ;
    string nuevaDesc   = g->descripcion ;
    float  nuevaX      = g->x ;
    float  nuevaY      = g->y ;
    float  nuevaZ      = g->z ;

    string entrada ;

    // codigo (debe ser unico entre las demas galaxias; vacio = mantener).
    while (true) {
        cout << "codigo (actual: " << g->codigo << ") -> nuevo codigo: " ;
        getline(cin, entrada) ;
        if (entrada.empty()) break ;           // mantener el valor actual
        Galaxia* otra = Principal.buscarGalaxiaPorCodigo(entrada) ;
        if (otra != nullptr && otra->id != g->id) {
            cout << ROJO << "Ya existe otra galaxia con ese codigo." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        nuevoCodigo = entrada ;
        break ;
    }

    // nombre.
    cout << "nombre (actual: " << g->nombre << ") -> nuevo nombre: " ;
    getline(cin, entrada) ;
    if (!entrada.empty()) nuevoNombre = entrada ;

    // tipo.
    cout << "tipo (actual: " << g->tipo << ") -> nuevo tipo: " ;
    getline(cin, entrada) ;
    if (!entrada.empty()) nuevoTipo = entrada ;

    // descripcion.
    cout << "descripcion (actual: " << g->descripcion << ") -> nueva descripcion: " ;
    getline(cin, entrada) ;
    if (!entrada.empty()) nuevaDesc = entrada ;

    // x (numero, vacio = mantener).
    while (true) {
        cout << "x (actual: " << g->x << ") -> nueva x: " ;
        getline(cin, entrada) ;
        if (entrada.empty()) break ;           // mantener el valor actual
        try { nuevaX = stof(entrada) ; break ; }
        catch (...) {
            cout << ROJO << "Valor invalido." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
        }
    }

    // y.
    while (true) {
        cout << "y (actual: " << g->y << ") -> nueva y: " ;
        getline(cin, entrada) ;
        if (entrada.empty()) break ;
        try { nuevaY = stof(entrada) ; break ; }
        catch (...) {
            cout << ROJO << "Valor invalido." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
        }
    }

    // z.
    while (true) {
        cout << "z (actual: " << g->z << ") -> nueva z: " ;
        getline(cin, entrada) ;
        if (entrada.empty()) break ;
        try { nuevaZ = stof(entrada) ; break ; }
        catch (...) {
            cout << ROJO << "Valor invalido." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
        }
    }

    // Guardar los nuevos valores (el id no cambia).
    g->Editar(nuevoCodigo, nuevoNombre, nuevoTipo, nuevaDesc, nuevaX, nuevaY, nuevaZ) ;

    // Cambiaron datos del grafo: reconstruir adyacencia y recalcular Kruskal.
    Principal.crearListaAdyacencia() ;
    k = kruskal(Principal) ;
    k.crearListaAdyacencia() ;

    cout << endl ;
    cout << VERDE << "Galaxia '" << g->id << "' actualizada correctamente:" << RESET << endl ;
    g->print() ;
    this_thread::sleep_for(chrono::seconds(2));
}

void smEliminarGalaxia() {
    Galaxia* g = nullptr ;

    // Listar las galaxias y pedir cual eliminar (numero -> "galaxia-N").
    while (true) {
        cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
        cout << ITALICA << ROJO_CLARO << "---------- Eliminar Galaxia ----------" << RESET << endl ;
        cout << endl ;
        Principal.printGalaxias() ;
        cout << endl ;
        cout << "Escriba el numero de la galaxia que desea eliminar (0 para volver): " ;

        int o = leerOpcion() ;
        if (o == 0) {
            return ;
        }
        if (o < 0) continue ;   // leerOpcion ya aviso de la entrada invalida
        string id = "galaxia-" + to_string(o) ;
        g = Principal.buscarGalaxia(id) ;
        if (g == nullptr) {
            cout << ROJO << "La galaxia no existe." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        break ;
    }

    // Mostrar la galaxia seleccionada.
    cout << endl ;
    g->print() ;
    cout << endl ;

    // Guardar el id ANTES de borrar (luego el puntero queda colgando).
    string id = g->id ;

    // Avisar que tambien se borran sus rutas y confirmar.
    cout << ROJO_CLARO << "Al eliminar la galaxia '" << id
         << "' tambien se borraran TODAS sus rutas conectadas." << RESET << endl ;
    cout << ROJO_CLARO << "Seguro que desea eliminarla? (si/no): " << RESET ;
    string resp ;
    getline(cin, resp) ;
    if (!(resp == "si" || resp == "Si" || resp == "SI" || resp == "s" || resp == "S")) {
        cout << AMARILLO << "Eliminacion cancelada." << RESET << endl ;
        this_thread::sleep_for(chrono::seconds(1));
        return ;
    }

    // Eliminar la galaxia (y sus rutas) del grafo principal.
    Principal.eliminarGalaxia(id) ;

    // Recalcular adyacencia del principal y el grafo optimizado (Kruskal).
    Principal.crearListaAdyacencia() ;
    k = kruskal(Principal) ;
    k.crearListaAdyacencia() ;

    cout << endl ;
    cout << VERDE << "Galaxia '" << id << "' eliminada con exito." << RESET << endl ;
    this_thread::sleep_for(chrono::seconds(2));
}

void smVerGalaxias () {
    int o ;

    do {
        cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
        cout << ITALICA << MAGENTA_CLARO << "---------- Ver Galaxias ----------" << RESET << endl ;
        cout << endl ;
        Principal.printGalaxias() ;
        cout << endl ;
        cout << "Escriba el numero de la galaxia a ver (0 para volver): " ;

        o = leerOpcion() ;
        if (o == 0) {
            break ;
        }
        if (o < 0) continue ;   // leerOpcion ya aviso de la entrada invalida

        string id = "galaxia-" + to_string(o) ;
        Galaxia* g = Principal.buscarGalaxia(id) ;
        if (g == nullptr) {
            cout << ROJO << "La galaxia no existe." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }

        cout << endl ;
        g->print() ;
        this_thread::sleep_for(chrono::seconds(2));
        break ;

    } while (true) ;
}

void smenuGalaxias () {
    int  o ;

    do  {
        cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
        cout  <<ITALICA<<MAGENTA_CLARO << "--------- SubMenu de Galaxias --------"<<RESET<<endl ;
        cout<<endl ;
        cout <<"1. Crear Galaxia "<<endl ;
        cout <<"2. Editar Galaxia "<<endl ;
        cout <<"3. Eliminar Galaxia "<<endl ;
        cout <<"4. Ver Galaxias "<<endl ;
        cout <<"0. Volver "<<endl ;cout<<endl ;
        cout <<"Escriba una opcion : " ;

        o= leerOpcion () ;


        if (o < 0 || o > 4) {
            cout << ROJO << "Solo opciones del 0 al 4.\n" << RESET;
            this_thread::sleep_for(chrono::seconds(1));
            continue;
        }

        switch  (o) { 

            case 1 : { 
                smCrearGalaxia() ;
                continue;
            }
            case 2 :{ 
                smEditarGalaxia() ;
                continue;
            }

            case 3 : {
                smEliminarGalaxia() ;
                continue;
            }

            case 4 :  { 
                smVerGalaxias () ; 
                continue;
            }

        }

    }while (o!= 0) ;
    
} ;


//*--------------------------------------------------------------//--------------------------------------------------------------




void smCrearRuta () {
    // Variables de la nueva ruta.
    string origen_id ;
    string destino_id ;
    string tipo ;
    float  costo = 0.0f ;
    float  tiempo_dias = 0.0f ;
    bool   activa = false ;

    // Titulo + tabla de galaxias disponibles para escoger origen/destino.
    cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
    cout << NEGRITA << VERDE_CLARO << "============ Crear Nueva Ruta ============" << RESET << endl ;
    cout << endl ;
    cout << CIAN << "--------------Galaxias disponibles--------------" << RESET << endl ;
    cout << endl ;
    Principal.printGalaxias() ;
    cout << endl ;

    // 1) Galaxia ORIGEN (se pide el numero y se arma el id "galaxia-N").
    while (true) {
        cout << "Escriba el numero de la galaxia ORIGEN (0 para cancelar): " ;
        int g = leerOpcion() ;
        if (g == 0) {
            cout << AMARILLO << "Creacion de ruta cancelada." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            return ;
        }
        if (g < 0) continue ;   // leerOpcion ya aviso de la entrada invalida
        string id = "galaxia-" + to_string(g) ;
        if (Principal.getGalaxia(id).id == "") {
            cout << ROJO << "Esa galaxia no existe. Intente de nuevo." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        origen_id = id ;
        break ;
    }

    // 2) Galaxia DESTINO (debe existir y ser distinta del origen).
    while (true) {
        cout << "Escriba el numero de la galaxia DESTINO (0 para cancelar): " ;
        int g = leerOpcion() ;
        if (g == 0) {
            cout << AMARILLO << "Creacion de ruta cancelada." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            return ;
        }
        if (g < 0) continue ;
        string id = "galaxia-" + to_string(g) ;
        if (Principal.getGalaxia(id).id == "") {
            cout << ROJO << "Esa galaxia no existe. Intente de nuevo." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        if (id == origen_id) {
            cout << ROJO << "El destino no puede ser igual al origen." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        destino_id = id ;
        break ;
    }

    // 3) Tipo de ruta (texto, no puede quedar vacio).
    while (true) {
        cout << "Escriba el tipo de ruta (ej: comercial, militar): " ;
        getline(cin, tipo) ;
        if (tipo.empty()) {
            cout << ROJO << "El tipo no puede quedar vacio." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        break ;
    }

    // 4) Costo (float >= 0).
    while (true) {
        cout << "Escriba el costo de la ruta (>= 0): " ;
        if (cin >> costo && costo >= 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
            break ;
        }
        cin.clear() ;
        cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
        cout << ROJO << "Costo invalido. Ingrese un numero >= 0." << RESET << endl ;
        this_thread::sleep_for(chrono::seconds(1));
    }

    // 5) Tiempo en dias (float >= 0).
    while (true) {
        cout << "Escriba el tiempo en dias (>= 0): " ;
        if (cin >> tiempo_dias && tiempo_dias >= 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
            break ;
        }
        cin.clear() ;
        cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
        cout << ROJO << "Tiempo invalido. Ingrese un numero >= 0." << RESET << endl ;
        this_thread::sleep_for(chrono::seconds(1));
    }

    // 6) Activa? (si / no).
    while (true) {
        cout << "La ruta esta activa? (s/n): " ;
        string resp ;
        getline(cin, resp) ;
        if (resp == "s" || resp == "S" || resp == "si" || resp == "Si" || resp == "SI") {
            activa = true ;
            break ;
        }
        if (resp == "n" || resp == "N" || resp == "no" || resp == "No" || resp == "NO") {
            activa = false ;
            break ;
        }
        cout << ROJO << "Responda 's' (si) o 'n' (no)." << RESET << endl ;
        this_thread::sleep_for(chrono::seconds(1));
    }

    // Generar un id unico para la ruta ("ruta-N" sin chocar con los existentes).
    int n = (int)Principal.rutas.size() + 1 ;
    string nuevoId = "ruta-" + to_string(n) ;
    while (Principal.buscarRuta(nuevoId) != nullptr) {
        n++ ;
        nuevoId = "ruta-" + to_string(n) ;
    }

    // Crear el objeto Ruta y agregarlo al grafo principal.
    Ruta nueva(nuevoId, origen_id, destino_id, tipo, costo, tiempo_dias, activa) ;
    if (!Principal.agregarRuta(nueva)) {
        cout << ROJO << "No se pudo crear la ruta (id repetido u origen/destino invalido)." << RESET << endl ;
        this_thread::sleep_for(chrono::seconds(2));
        return ;
    }

    // agregarRuta ya reconstruye la lista de adyacencia del grafo principal;
    // recalculamos el arbol de expansion minima (Kruskal) con la nueva ruta.
    k = kruskal(Principal) ;
    k.crearListaAdyacencia() ;

    cout << endl ;
    cout << VERDE << "Ruta '" << nuevoId << "' creada correctamente:" << RESET << endl ;
    nueva.print() ;
    this_thread::sleep_for(chrono::seconds(2));
}


void smEditarRuta() {
    Ruta* r = nullptr ;

    // Seleccionar la ruta a editar: se listan y se pide el numero -> "ruta-N".
    while (true) {
        cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
        cout << ITALICA << VERDE_CLARO << "---------- Editar Ruta ----------" << RESET << endl ;
        cout << endl ;
        Principal.printRutas() ;
        cout << endl ;
        cout << "Escriba el numero de la ruta a editar (0 para volver): " ;

        int o = leerOpcion() ;
        if (o == 0) {
            return ;
        }
        if (o < 0) continue ;   // leerOpcion ya aviso de la entrada invalida
        string id = "ruta-" + to_string(o) ;
        r = Principal.buscarRuta(id) ;
        if (r == nullptr) {                       // puntero vacio -> no existe
            cout << ROJO << "La ruta no existe." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        break ;
    }

    cout << endl ;
    cout << CIAN << "--------------Galaxias disponibles--------------" << RESET << endl ;
    cout << endl ;
    Principal.printGalaxias() ;

    // Mostrar la ruta encontrada con formato claro.
    cout << endl ;
    cout << VERDE << "Editando la siguiente ruta:" << RESET << endl ;
    r->print() ;
    cout << endl ;
    cout << AMARILLO << "Para cada campo escriba el nuevo valor. Enter VACIO mantiene el valor actual." << RESET << endl ;
    cout << endl ;

    // Variables para los nuevos valores (parten del valor actual).
    string nuevoOrigen  = r->origen_id ;
    string nuevoDestino = r->destino_id ;
    string nuevoTipo    = r->tipo ;
    float  nuevoCosto   = r->costo ;
    float  nuevoTiempo  = r->tiempo_dias ;
    bool   nuevaActiva  = r->activa ;

    string entrada ;

    // origen_id (se pide numero de galaxia; debe existir).
    while (true) {
        cout << "origen_id (actual: " << r->origen_id << ") -> nuevo numero de galaxia: " ;
        getline(cin, entrada) ;
        if (entrada.empty()) {           // Enter vacio -> mantener el valor actual
            break ;
        }
        int g ;
        try { g = stoi(entrada) ; }
        catch (...) {
            cout << ROJO << "Numero invalido." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        string id = "galaxia-" + to_string(g) ;
        if (Principal.getGalaxia(id).id == "") {
            cout << ROJO << "Esa galaxia no existe." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        nuevoOrigen = id ;
        break ;
    }

    // destino_id (debe existir y ser distinto del nuevo origen).
    while (true) {
        cout << "destino_id (actual: " << r->destino_id << ") -> nuevo numero de galaxia: " ;
        getline(cin, entrada) ;
        if (entrada.empty()) {           // Enter vacio -> mantener el valor actual
            break ;
        }
        int g ;
        try { g = stoi(entrada) ; }
        catch (...) {
            cout << ROJO << "Numero invalido." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        string id = "galaxia-" + to_string(g) ;
        if (Principal.getGalaxia(id).id == "") {
            cout << ROJO << "Esa galaxia no existe." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        if (id == nuevoOrigen) {
            cout << ROJO << "El destino no puede ser igual al origen." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        nuevoDestino = id ;
        break ;
    }

    // tipo (texto, no puede quedar vacio porque vacio = cancelar).
    while (true) {
        cout << "tipo (actual: " << r->tipo << ") -> nuevo tipo: " ;
        getline(cin, entrada) ;
        if (entrada.empty()) {           // Enter vacio -> mantener el valor actual
            break ;
        }
        nuevoTipo = entrada ;
        break ;
    }

    // costo (float >= 0).
    while (true) {
        cout << "costo (actual: " << r->costo << ") -> nuevo costo: " ;
        getline(cin, entrada) ;
        if (entrada.empty()) {           // Enter vacio -> mantener el valor actual
            break ;
        }
        float v ;
        try { v = stof(entrada) ; }
        catch (...) {
            cout << ROJO << "Valor invalido." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        if (v < 0) {
            cout << ROJO << "El costo debe ser >= 0." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        nuevoCosto = v ;
        break ;
    }

    // tiempo_dias (float >= 0).
    while (true) {
        cout << "tiempo_dias (actual: " << r->tiempo_dias << ") -> nuevo tiempo: " ;
        getline(cin, entrada) ;
        if (entrada.empty()) {           // Enter vacio -> mantener el valor actual
            break ;
        }
        float v ;
        try { v = stof(entrada) ; }
        catch (...) {
            cout << ROJO << "Valor invalido." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        if (v < 0) {
            cout << ROJO << "El tiempo debe ser >= 0." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        nuevoTiempo = v ;
        break ;
    }

    // activa (s/n).
    while (true) {
        cout << "activa (actual: " << (r->activa ? "si" : "no") << ") -> (s/n): " ;
        getline(cin, entrada) ;
        if (entrada.empty()) {           // Enter vacio -> mantener el valor actual
            break ;
        }
        if (entrada == "s" || entrada == "S" || entrada == "si" || entrada == "Si" || entrada == "SI") {
            nuevaActiva = true ;
            break ;
        }
        if (entrada == "n" || entrada == "N" || entrada == "no" || entrada == "No" || entrada == "NO") {
            nuevaActiva = false ;
            break ;
        }
        cout << ROJO << "Responda 's' (si) o 'n' (no)." << RESET << endl ;
        this_thread::sleep_for(chrono::seconds(1));
    }

    // Guardar los nuevos valores en la ruta (el id no cambia).
    r->Editar(nuevoOrigen, nuevoDestino, nuevoTipo, nuevoCosto, nuevoTiempo, nuevaActiva) ;

    // Cambiaron datos del grafo: reconstruir adyacencia y recalcular Kruskal.
    Principal.crearListaAdyacencia() ;
    k = kruskal(Principal) ;
    k.crearListaAdyacencia() ;

    cout << endl ;
    cout << VERDE << "Ruta '" << r->id << "' actualizada correctamente:" << RESET << endl ;
    r->print() ;
    this_thread::sleep_for(chrono::seconds(2));


}

void smEliminarRuta() {
    Ruta* r = nullptr ;

    // Listar las rutas actuales y pedir cual eliminar (numero -> "ruta-N").
    while (true) {
        cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
        cout << ITALICA << ROJO_CLARO << "---------- Eliminar Ruta ----------" << RESET << endl ;
        cout << endl ;
        Principal.printRutas() ;
        cout << endl ;
        cout <<AZUL_CLARO<< "Escriba el numero de la ruta que desea eliminar (0 para volver): "<<RESET ;

        int o = leerOpcion() ;
        if (o == 0) {
            return ;
        }
        if (o < 0) continue ;   // leerOpcion ya aviso de la entrada invalida
        string id = "ruta-" + to_string(o) ;
        r = Principal.buscarRuta(id) ;
        if (r == nullptr) {
            cout << ROJO << "La ruta no existe." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        break ;
    }

    // Mostrar la ruta seleccionada.
    cout << endl ;
    r->print() ;
    cout << endl ;

    // Guardar el id ANTES de borrar (luego el puntero queda colgando).
    string id = r->id ;

    // Confirmar. Cualquier respuesta distinta de "si" cancela.
    cout << ROJO_CLARO << "Seguro que desea eliminar la ruta '" << id << "'? (si/no): " << RESET ;
    string resp ;
    getline(cin, resp) ;
    if (!(resp == "si" || resp == "Si" || resp == "SI" || resp == "s" || resp == "S")) {
        cout << AMARILLO << "Eliminacion cancelada." << RESET << endl ;
        this_thread::sleep_for(chrono::seconds(1));
        return ;
    }

    // Eliminar la ruta del grafo principal.
    Principal.eliminarRuta(id) ;

    // Recalcular adyacencia del principal y el grafo optimizado (Kruskal).
    Principal.crearListaAdyacencia() ;
    k = kruskal(Principal) ;
    k.crearListaAdyacencia() ;

    cout << endl ;
    cout << VERDE << "Ruta '" << id << "' eliminada con exito." << RESET << endl ;
    this_thread::sleep_for(chrono::seconds(2));
}



void smverRutas() {
    int o ;

    do {
        cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
        cout << ITALICA << VERDE_CLARO << "---------- Ver Rutas ----------" << RESET << endl ;
        cout << endl ;
        Principal.printRutas() ;
        cout << endl ;
        cout << "Escriba el numero de la ruta a ver (0 para volver): " ;

        o = leerOpcion() ;
        if (o == 0) {
            break ;
        }
        if (o < 0) continue ;   // leerOpcion ya aviso de la entrada invalida

        string id = "ruta-" + to_string(o) ;
        Ruta* r = Principal.buscarRuta(id) ;
        if (r == nullptr) {
            cout << ROJO << "La ruta no existe." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }

        cout << endl ;
        r->print() ;
        this_thread::sleep_for(chrono::seconds(2));
        break ;

    } while (true) ;
} ;

void smenuRutas() {
    int o ;

    do {
        cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
        cout  <<ITALICA<<VERDE_CLARO << "---------- SubMenu de Rutas ----------"<<RESET<<endl ;
        cout<<endl ;
        cout <<"1. Crear Ruta "<<endl ; 
        cout <<"2. Editar Ruta "<<endl ; 
        cout <<"3. Eliminar Ruta "<<endl ; 
        cout <<"4. Ver Rutas "<<endl ; 
        cout <<"0. Volver "<<endl ;cout<<endl ;
        cout <<"Escriba una opcion : " ; 

        o= leerOpcion () ;

        if (o < 0 || o > 4) {
            cout << ROJO << "Solo opciones del 0 al 4.\n" << RESET;
            this_thread::sleep_for(chrono::seconds(1));
            continue;
        }

        switch  (o) { 

            case 1 : { 
                smCrearRuta() ;
                continue;

            }
            case 2 :{ 
                smEditarRuta () ;
                continue;

            }

            case 3 : {
                smEliminarRuta() ;
                continue;
                
            }
            case 4  :{
                smverRutas() ;
                continue;

            }

        }

    }while (o!= 0) ;
} ; 



//*--------------------------------------------------------------//--------------------------------------------------------------

void smCrearNaves() {
    // Variables de la nueva nave.
    string codigo ;
    string nombre ;
    int    capacidad = 0 ;
    int    velocidad_Max = 0 ;
    bool   activa = false ;

    cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
    cout << NEGRITA << AZUL_CLARO << "============ Crear Nueva Nave ============" << RESET << endl ;
    cout << endl ;

    // 1) Codigo (texto, no puede quedar vacio).
    while (true) {
        cout << "Escriba el codigo de la nave: " ;
        getline(cin, codigo) ;
        if (codigo.empty()) {
            cout << ROJO << "El codigo no puede quedar vacio." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        break ;
    }

    // 2) Nombre (texto, no puede quedar vacio).
    while (true) {
        cout << "Escriba el nombre de la nave: " ;
        getline(cin, nombre) ;
        if (nombre.empty()) {
            cout << ROJO << "El nombre no puede quedar vacio." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        break ;
    }

    // 3) Capacidad (entero >= 0).
    while (true) {
        cout << "Escriba la capacidad de la nave (>= 0): " ;
        if (cin >> capacidad && capacidad >= 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
            break ;
        }
        cin.clear() ;
        cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
        cout << ROJO << "Capacidad invalida. Ingrese un entero >= 0." << RESET << endl ;
        this_thread::sleep_for(chrono::seconds(1));
    }

    // 4) Velocidad maxima (entero >= 0).
    while (true) {
        cout << "Escriba la velocidad maxima de la nave (>= 0): " ;
        if (cin >> velocidad_Max && velocidad_Max >= 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
            break ;
        }
        cin.clear() ;
        cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
        cout << ROJO << "Velocidad invalida. Ingrese un entero >= 0." << RESET << endl ;
        this_thread::sleep_for(chrono::seconds(1));
    }

    // 5) Activa? (si / no).
    while (true) {
        cout << "La nave esta activa? (s/n): " ;
        string resp ;
        getline(cin, resp) ;
        if (resp == "s" || resp == "S" || resp == "si" || resp == "Si" || resp == "SI") {
            activa = true ;
            break ;
        }
        if (resp == "n" || resp == "N" || resp == "no" || resp == "No" || resp == "NO") {
            activa = false ;
            break ;
        }
        cout << ROJO << "Responda 's' (si) o 'n' (no)." << RESET << endl ;
        this_thread::sleep_for(chrono::seconds(1));
    }

    // Generar un id unico ("nave-N" sin chocar con las existentes).
    int n = (int)naves.size() + 1 ;
    string nuevoId = "nave-" + to_string(n) ;
    while (buscarNave(naves, nuevoId).id != "") {
        n++ ;
        nuevoId = "nave-" + to_string(n) ;
    }

    // Crear la nave y agregarla al vector global de naves.
    Nave nueva(nuevoId, codigo, nombre, capacidad, velocidad_Max, activa) ;
    naves.push_back(nueva) ;

    cout << endl ;
    cout << VERDE << "Nave '" << nuevoId << "' creada correctamente:" << RESET << endl ;
    nueva.print() ;
    this_thread::sleep_for(chrono::seconds(2));
}


void smEditarNaves() {
    Nave* nv = nullptr ;

    // Seleccionar la nave a editar: se listan y se pide el numero -> "nave-N".
    while (true) {
        cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
        cout << ITALICA << AZUL_CLARO << "---------- Editar Nave ----------" << RESET << endl ;
        cout << endl ;
        imprimirNaves(naves) ;
        cout << endl ;
        cout << "Escriba el numero de la nave a editar (0 para volver): " ;

        int o = leerOpcion() ;
        if (o == 0) {
            return ;
        }
        if (o < 0) continue ;   // leerOpcion ya aviso de la entrada invalida
        string id = "nave-" + to_string(o) ;
        for (Nave &n : naves) {            // buscar la referencia real en el vector
            if (n.id == id) { nv = &n ; break ; }
        }
        if (nv == nullptr) {
            cout << ROJO << "La nave no existe." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        break ;
    }

    // Mostrar la nave encontrada.
    cout << endl ;
    cout << VERDE << "Editando la siguiente nave:" << RESET << endl ;
    nv->print() ;
    cout << endl ;
    cout << AMARILLO << "Para cada campo escriba el nuevo valor. Enter VACIO mantiene el valor actual." << RESET << endl ;
    cout << endl ;

    // Variables para los nuevos valores (parten del valor actual).
    string nuevoCodigo    = nv->codigo ;
    string nuevoNombre    = nv->nombre ;
    int    nuevaCapacidad = nv->capacidad ;
    int    nuevaVelocidad = nv->velocidad_Max ;
    bool   nuevaActiva    = nv->activa ;

    string entrada ;

    // codigo.
    cout << "codigo (actual: " << nv->codigo << ") -> nuevo codigo: " ;
    getline(cin, entrada) ;
    if (!entrada.empty()) nuevoCodigo = entrada ;

    // nombre.
    cout << "nombre (actual: " << nv->nombre << ") -> nuevo nombre: " ;
    getline(cin, entrada) ;
    if (!entrada.empty()) nuevoNombre = entrada ;

    // capacidad (entero >= 0, vacio = mantener).
    while (true) {
        cout << "capacidad (actual: " << nv->capacidad << ") -> nueva capacidad: " ;
        getline(cin, entrada) ;
        if (entrada.empty()) break ;           // mantener el valor actual
        int v ;
        try { v = stoi(entrada) ; }
        catch (...) {
            cout << ROJO << "Valor invalido." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        if (v < 0) {
            cout << ROJO << "La capacidad debe ser >= 0." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        nuevaCapacidad = v ;
        break ;
    }

    // velocidad_Max (entero >= 0, vacio = mantener).
    while (true) {
        cout << "velocidad_Max (actual: " << nv->velocidad_Max << ") -> nueva velocidad: " ;
        getline(cin, entrada) ;
        if (entrada.empty()) break ;
        int v ;
        try { v = stoi(entrada) ; }
        catch (...) {
            cout << ROJO << "Valor invalido." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        if (v < 0) {
            cout << ROJO << "La velocidad debe ser >= 0." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        nuevaVelocidad = v ;
        break ;
    }

    // activa (s/n, vacio = mantener).
    while (true) {
        cout << "activa (actual: " << (nv->activa ? "si" : "no") << ") -> (s/n): " ;
        getline(cin, entrada) ;
        if (entrada.empty()) break ;           // mantener el valor actual
        if (entrada == "s" || entrada == "S" || entrada == "si" || entrada == "Si" || entrada == "SI") {
            nuevaActiva = true ;
            break ;
        }
        if (entrada == "n" || entrada == "N" || entrada == "no" || entrada == "No" || entrada == "NO") {
            nuevaActiva = false ;
            break ;
        }
        cout << ROJO << "Responda 's' (si) o 'n' (no)." << RESET << endl ;
        this_thread::sleep_for(chrono::seconds(1));
    }

    // Guardar los nuevos valores en la nave (el id no cambia).
    nv->Editar(nuevoCodigo, nuevoNombre, nuevaCapacidad, nuevaVelocidad, nuevaActiva) ;

    cout << endl ;
    cout << VERDE << "Nave '" << nv->id << "' actualizada correctamente:" << RESET << endl ;
    nv->print() ;
    this_thread::sleep_for(chrono::seconds(2));
}

void smEliminarNaves() {
    string id ;

    // Listar las naves y pedir cual eliminar (numero -> "nave-N").
    while (true) {
        cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
        cout << ITALICA << ROJO_CLARO << "---------- Eliminar Nave ----------" << RESET << endl ;
        cout << endl ;
        imprimirNaves(naves) ;
        cout << endl ;
        cout << "Escriba el numero de la nave que desea eliminar (0 para volver): " ;

        int o = leerOpcion() ;
        if (o == 0) {
            return ;
        }
        if (o < 0) continue ;   // leerOpcion ya aviso de la entrada invalida
        id = "nave-" + to_string(o) ;
        if (buscarNave(naves, id).id == "") {     // no existe ninguna nave con ese id
            cout << ROJO << "La nave no existe." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        break ;
    }

    // Mostrar la nave seleccionada (buscandola por id, no por posicion).
    cout << endl ;
    buscarNave(naves, id).print() ;
    cout << endl ;

    // Confirmar. Cualquier respuesta distinta de "si" cancela.
    cout << ROJO_CLARO << "Seguro que desea eliminar la nave '" << id << "'? (si/no): " << RESET ;
    string resp ;
    getline(cin, resp) ;
    if (!(resp == "si" || resp == "Si" || resp == "SI" || resp == "s" || resp == "S")) {
        cout << AMARILLO << "Eliminacion cancelada." << RESET << endl ;
        this_thread::sleep_for(chrono::seconds(1));
        return ;
    }

    // Eliminar la nave buscandola por su id (no por indice/offset).
    for (auto it = naves.begin(); it != naves.end(); ++it) {
        if (it->id == id) {
            naves.erase(it) ;
            break ;
        }
    }

    cout << endl ;
    cout << VERDE << "Nave '" << id << "' eliminada con exito." << RESET << endl ;
    this_thread::sleep_for(chrono::seconds(2));
}

void smregistrarViaje() {
    // Datos del viaje a registrar.
    string nave_id ;          // id de la nave escogida
    string ruta_id ;          // id de la ruta escogida
    string fecha ;            // fecha del viaje
    float  costo_real = 0.0f ;
    bool   exitoso = false ;
    string nave_nombre ;      // nombre de la nave escogida
    string origen_nombre ;    // nombre de la galaxia origen
    string destino_nombre ;   // nombre de la galaxia destino

    // ===================== 1) ESCOGER NAVE =====================
    cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
    cout << NEGRITA << VERDE_CLARO << "============ Registrar Viaje ============" << RESET << endl ;
    cout << endl ;
    cout << CIAN << "--------------Naves disponibles--------------" << RESET << endl ;
    cout << endl ;
    imprimirNaves(naves) ;
    cout << endl ;

    while (true) {
        cout << "Escriba el numero de la nave (0 para cancelar): " ;
        int o = leerOpcion() ;
        if (o == 0) {
            cout << AMARILLO << "Registro de viaje cancelado." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            return ;
        }
        if (o < 0) continue ;   // leerOpcion ya aviso de la entrada invalida
        string id = "nave-" + to_string(o) ;
        Nave nv = buscarNave(naves, id) ;
        if (nv.id == "") {
            cout << ROJO << "Esa nave no existe. Intente de nuevo." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        nave_id = nv.id ;
        nave_nombre = nv.nombre ;
        break ;
    }

    // ===================== 2) ESCOGER GALAXIA ORIGEN =====================
    string origen_id ;
    cout << endl ;
    cout << CIAN << "--------------Galaxias disponibles--------------" << RESET << endl ;
    cout << endl ;
    Principal.printGalaxias() ;
    cout << endl ;

    while (true) {
        cout << "Escriba el numero de la galaxia ORIGEN (0 para cancelar): " ;
        int g = leerOpcion() ;
        if (g == 0) {
            cout << AMARILLO << "Registro de viaje cancelado." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            return ;
        }
        if (g < 0) continue ;
        string id = "galaxia-" + to_string(g) ;
        Galaxia gal = Principal.getGalaxia(id) ;
        if (gal.id == "") {
            cout << ROJO << "Esa galaxia no existe. Intente de nuevo." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        origen_id = gal.id ;
        origen_nombre = gal.nombre ;
        break ;
    }

    // ===================== 3) MOSTRAR RUTAS DE ESA GALAXIA =====================
    Galaxia gOrigen = Principal.getGalaxia(origen_id) ;

    // Rutas disponibles para escoger (las del grafo principal que tocan el origen).
    vector<Ruta> rutasDisponibles = Principal.rutasDesdeGalaxia(origen_id) ;
    if (rutasDisponibles.empty()) {
        cout << endl ;
        cout << ROJO << "La galaxia '" << origen_id << "' no tiene rutas. No se puede registrar el viaje." << RESET << endl ;
        this_thread::sleep_for(chrono::seconds(2));
        return ;
    }

    cout << endl ;
    cout << CIAN << "----------- Grafo Principal (todas las rutas) -----------" << RESET << endl ;
    Principal.imprimirAdyacencia(gOrigen) ;
    cout << endl ;
    cout << CIAN << "----------- Grafo Kruskal (expansion minima) -----------" << RESET << endl ;
    k.imprimirAdyacencia(gOrigen) ;
    cout << endl ;

    // ===================== 4) ESCOGER RUTA Y OBTENER DESTINO =====================
    Ruta rutaSel ;
    while (true) {
        cout << "Escriba el numero de la ruta a tomar (0 para cancelar): " ;
        int o = leerOpcion() ;
        if (o == 0) {
            cout << AMARILLO << "Registro de viaje cancelado." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            return ;
        }
        if (o < 0) continue ;
        string id = "ruta-" + to_string(o) ;

        // La ruta debe existir Y tocar la galaxia origen (estar entre las mostradas).
        bool valida = false ;
        for (const Ruta &r : rutasDisponibles) {
            if (r.id == id) { rutaSel = r ; valida = true ; break ; }
        }
        if (!valida) {
            cout << ROJO << "Esa ruta no existe o no parte de la galaxia origen. Intente de nuevo." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        ruta_id = rutaSel.id ;
        break ;
    }

    // El destino es el otro extremo de la ruta (grafo no dirigido).
    string destino_id = (rutaSel.origen_id == origen_id) ? rutaSel.destino_id : rutaSel.origen_id ;
    destino_nombre = Principal.getGalaxia(destino_id).nombre ;

    // ===================== 5) FECHA =====================
    while (true) {
        cout << "Escriba la fecha del viaje (ej: 2026-06-23): " ;
        getline(cin, fecha) ;
        if (fecha.empty()) {
            cout << ROJO << "La fecha no puede quedar vacia." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }
        break ;
    }

    // ===================== 6) COSTO REAL (con costo recomendado) =====================
    cout << AMARILLO << "Costo recomendado (costo de la ruta): " << rutaSel.costo << RESET << endl ;
    while (true) {
        cout << "Escriba el costo real del viaje (>= 0): " ;
        if (cin >> costo_real && costo_real >= 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
            break ;
        }
        cin.clear() ;
        cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
        cout << ROJO << "Costo invalido. Ingrese un numero >= 0." << RESET << endl ;
        this_thread::sleep_for(chrono::seconds(1));
    }


    while (true) {
        cout << "El viaje fue exitoso? (s/n): " ;
        string resp ;
        getline(cin, resp) ;
        if (resp == "s" || resp == "S" || resp == "si" || resp == "Si" || resp == "SI") {
            exitoso = true ;
            break ;
        }
        if (resp == "n" || resp == "N" || resp == "no" || resp == "No" || resp == "NO") {
            exitoso = false ;
            break ;
        }
        cout << ROJO << "Responda 's' (si) o 'n' (no)." << RESET << endl ;
        this_thread::sleep_for(chrono::seconds(1));
    }

    int n = (int)viajes.size() + 1 ;
    string nuevoId = "viaje-" + to_string(n) ;
    bool repetido = true ;
    while (repetido) {
        repetido = false ;
        for (const Viaje &v : viajes) {
            if (v.id == nuevoId) { repetido = true ; break ; }
        }
        if (repetido) {
            n++ ;
            nuevoId = "viaje-" + to_string(n) ;
        }
    }

    Viaje nuevo(nuevoId, nave_id, ruta_id, fecha, costo_real, exitoso,
                nave_nombre, origen_nombre, destino_nombre) ;
    viajes.push_back(nuevo) ;

    // Recopiar a cada nave su historial (la nave escogida queda con su nuevo viaje).
    asignarHistorialANaves(naves, viajes) ;

    cout << endl ;
    cout << VERDE << "Viaje '" << nuevoId << "' registrado correctamente:" << RESET << endl ;
    nuevo.print() ;
    this_thread::sleep_for(chrono::seconds(2));
}

void smVerNaves () {
    int o ;

    do {
        cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
        cout << ITALICA << AZUL_CLARO << "---------- Ver Naves ----------" << RESET << endl ;
        cout << endl ;
        imprimirNaves(naves) ;
        cout << endl ;
        cout << "Escriba el numero de la nave a ver (0 para volver): " ;

        o = leerOpcion() ;
        if (o == 0) {
            break ;
        }
        if (o < 0) continue ;   // leerOpcion

        string id = "nave-" + to_string(o) ;
        Nave* nv = nullptr ;
        for (Nave &n : naves) {
            if (n.id == id) { nv = &n ; break ; }
        }
        if (nv == nullptr) {
            cout << ROJO << "La nave no existe." << RESET << endl ;
            this_thread::sleep_for(chrono::seconds(1));
            continue ;
        }

        cout << endl ;
        nv->print() ;
        this_thread::sleep_for(chrono::seconds(2));
        break ;

    } while (true) ;
}

void smenuNaves(){
    int  o ;

    do  {
        cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
        cout  <<ITALICA<<AZUL_CLARO << "-------- SubMenu de Naves --------"<<RESET<<endl ;
        cout<<endl ;
        cout <<"1. Crear Nave "<<endl ; 
        cout <<"2. Editar Nave "<<endl ; 
        cout <<"3. Eliminar Nave "<<endl ; 
        cout <<"4. Ver Naves "<<endl ; cout<<endl ;
        cout <<"5. Registrar Viaje "<<endl ; 
        cout <<"0. Volver "<<endl ;cout<<endl ;
        cout <<"Escriba una opcion : " ; 

        o= leerOpcion () ;

        if (o < 0 || o > 5) {
            cout << ROJO << "Solo opciones del 0 al 5.\n" << RESET;
            this_thread::sleep_for(chrono::seconds(1));
            continue;
        }

        switch  (o) {

            case 1 : {
                smCrearNaves () ;
                continue;

            }
            case 2 :{
                smEditarNaves () ;
                continue;

            }

            case 3 : {
                smEliminarNaves () ;
                continue;

            }

            case 4 : { 
                smVerNaves () ; 
                continue;

            }

            case 5 : {
                smregistrarViaje () ; 
                continue;
            }

        }

    }while (o!= 0) ;
} ; 







void menuGrafo() { 
    int o ;
    do {
        cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
        cout  << AMARILLO << "========= Menu de Grafo ========="<<RESET<<endl ;
        cout<<endl ;
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

        switch ( o)
        {
            case 1 :{ 
                smenuGalaxias() ;
                continue;

            }

            case 2 : {
                smenuRutas() ;
                continue;
                
            }

            case 3 : {
                smenuNaves() ;
                continue;

            }

        
        }
    }
    while (o!=0) ;

}















void menu () {

    int o; 
    do {
        cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;cout<<endl ;
        cout <<NEGRITA<<MAJENTA<<"-----------SISTEMA DE MODELADO DE GALAXIAS-----------"<<RESET<<endl ; 
        cout<<endl ;
        cout<<endl ;
        cout  <<NEGRITA<< AZUL_CLARO << "============"<<MAJENTA<<" Menu principal"<<AZUL_CLARO<<" ============"<<RESET<<endl ; 
        cout<<endl ;
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

                // Antes de salir, guardar TODO el estado (grafo + naves + historial)
                // en NOMBRE_GRAFO para poder recargarlo despues sin la API.
                cout << AMARILLO << "Guardando datos en " << NOMBRE_GRAFO << "..." << RESET << endl;
                if (serializeAll(Principal, naves)) {
                    cout << VERDE << "Datos guardados en " << NOMBRE_GRAFO << RESET << endl;
                } else {
                    cout << ROJO << "No se pudo guardar " << NOMBRE_GRAFO << RESET << endl;
                }

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