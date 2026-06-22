#pragma once
#include "API.cpp"
#include <map>
#include <queue>
#include <limits>
#include <algorithm>
//!EL GRAFO ES NO DIRIJIDO
using namespace std  ;


struct UnionFind {
    map<string, string> padre;   // padre[id] = representante directo del nodo
    map<string, int>    rango;   // altura aproximada del arbol de cada raiz

    // Crea un conjunto distinto para cada nodo del grafo.
    void inicializar (const Grafo &g) {
        for (const Galaxia &galaxia : g.galaxias) {
            padre[galaxia.id] = galaxia.id;   // cada nodo es su propio padre
            rango[galaxia.id] = 0;
        }
    }

    // Devuelve el representante (raiz) del conjunto al que pertenece "id".
    // Aplica compresion de caminos: cuelga el nodo directo de la raiz.
    string find (const string &id) {
        // Si el id no existe todavia, lo registramos como su propio conjunto.
        if (padre.find(id) == padre.end()) {
            padre[id] = id;
            rango[id] = 0;
            return id;
        }

        if (padre[id] != id) {
            padre[id] = find(padre[id]);   // compresion de caminos
        }
        return padre[id];
    }

    // Une los conjuntos de "a" y "b". Devuelve false si ya estaban juntos.
    bool unir (const string &a, const string &b) {
        string raizA = find(a);
        string raizB = find(b);

        if (raizA == raizB) {
            return false;   // ya pertenecen al mismo conjunto -> formaria ciclo
        }

        // Union por rango: el arbol mas bajo cuelga del mas alto.
        if (rango[raizA] < rango[raizB]) {
            padre[raizA] = raizB;
        }
        else if (rango[raizA] > rango[raizB]) {
            padre[raizB] = raizA;
        }
        else {
            padre[raizB] = raizA;
            rango[raizA]++;
        }
        return true;
    }
};

Grafo kruskal (const Grafo &g) {
    Grafo mst;
    mst.galaxias   = g.galaxias;      // el MST tiene los mismos nodos
    mst.totalNodos = g.totalNodos;
    mst.esDirigido = g.esDirigido;
    mst.version    = g.version;

    vector<Ruta> rutasOrdenadas = g.rutas;
    sort(rutasOrdenadas.begin(), rutasOrdenadas.end(),
         [](const Ruta &a, const Ruta &b) {
             return a.costo < b.costo;
         });
    UnionFind conjuntos;
    conjuntos.inicializar(g);

    // 3. Recorrer los arcos en el orden (aleatorio) dado por la API.
    for (const Ruta &arco : g.rutas) {
        // a. Solo nos interesa si los extremos estan en conjuntos diferentes.
        if (conjuntos.unir(arco.origen_id, arco.destino_id)) {
            // b. Agregar el arco al grafo resultante.
            mst.rutas.push_back(arco);
        }
        // c. (unir() ya unio los conjuntos cuando devolvio true)

        // Un MST de N nodos tiene N-1 aristas: si ya las tenemos, terminamos.
        if ((int)mst.rutas.size() == mst.totalNodos - 1) {
            break;
        }
    }

    mst.totalAristas = (int)mst.rutas.size();
    return mst;
}


// ---------------------------------------------------------------------------
// Dijkstra
string otroExtremo (const Ruta &r, const string &nodo) {
    return (r.origen_id == nodo) ? r.destino_id : r.origen_id;
}


vector<Ruta> dijkstra (const Grafo &g, const string &origen, const string &destino) {
    // 1. Lista de adyacencia: por cada nodo, las rutas que lo tocan.
    //    Si el grafo ya tiene su listaAdyacencia construida (crearListaAdyacencia),
    //    la reusamos. Si no, la armamos aqui a partir de las rutas (fallback).
    map<string, vector<Ruta>> adyacencia;
    if (!g.listaAdyacencia.empty()) {
        for (const Adyacencia &a : g.listaAdyacencia) {
            adyacencia[a.galaxia.id] = a.rutas;
        }
    } else {
        // Como es no dirigido, cada ruta se agrega a sus DOS extremos.
        for (const Ruta &r : g.rutas) {
            adyacencia[r.origen_id].push_back(r);
            adyacencia[r.destino_id].push_back(r);
        }
    }

    // 2. Estructuras de Dijkstra.
    map<string, float> dist;         // mejor costo conocido para llegar a cada nodo
    map<string, Ruta>  rutaLlegada;  // arista con la que se llego a cada nodo
    map<string, bool>  visitado;

    for (const Galaxia &gal : g.galaxias) {
        dist[gal.id] = numeric_limits<float>::infinity();
    }
    dist[origen] = 0.0f;

    // Min-heap de pares (costoAcumulado, nodo): siempre saca el de menor costo.
    priority_queue<
        pair<float, string>,
        vector<pair<float, string>>,
        greater<pair<float, string>>
    > cola;
    cola.push({0.0f, origen});

    // 3. Bucle principal.
    while (!cola.empty()) {
        string actual = cola.top().second;
        cola.pop();

        if (visitado[actual]) continue;  // ya se proceso con su costo definitivo
        visitado[actual] = true;

        if (actual == destino) break;    // ya tenemos el costo optimo al destino

        // Relajar cada vecino: si llegar por aqui es mas barato, actualizar.
        for (const Ruta &r : adyacencia[actual]) {
            string vecino = otroExtremo(r, actual);
            float nuevoCosto = dist[actual] + r.costo;

            if (nuevoCosto < dist[vecino]) {
                dist[vecino] = nuevoCosto;
                rutaLlegada[vecino] = r;
                cola.push({nuevoCosto, vecino});
            }
        }
    }

    // 4. Reconstruir el camino caminando hacia atras desde el destino.
    vector<Ruta> camino;
    if (dist[destino] == numeric_limits<float>::infinity()) {
        return camino;   // destino inalcanzable -> vector vacio
    }

    string nodo = destino;
    while (nodo != origen) {
        if (rutaLlegada.find(nodo) == rutaLlegada.end()) {
            return vector<Ruta>();   // seguridad: camino incompleto
        }
        Ruta r = rutaLlegada[nodo];
        camino.push_back(r);
        nodo = otroExtremo(r, nodo);   // retroceder al nodo anterior
    }

    reverse(camino.begin(), camino.end());   // dejarlo en orden origen -> destino
    return camino;
}


// Imprime en formato tabla el camino que devuelve dijkstra: por cada paso muestra
// desde que galaxia, por que ruta y hacia que galaxia se va, con su costo y tiempo.
// "origen" es el id de la galaxia de partida (para saber por donde arranca).
void imprimirCamino (Grafo &g, const string &origen, const vector<Ruta> &camino) {
    if (camino.empty()) {
        cout << "No existe una ruta entre esas galaxias." << endl;
        return;
    }

    cout << left;
    cout << setw(20) << "DESDE" << setw(11) << "RUTA" << setw(20) << "HACIA"
         << setw(14) << "TIPO" << setw(10) << "COSTO" << setw(9) << "TIEMPO" << endl;
    cout << string(84, '-') << endl;

    string actual = origen;
    float costoTotal = 0.0f;
    float tiempoTotal = 0.0f;

    for (const Ruta &r : camino) {
        string siguiente = otroExtremo(r, actual);   // el otro extremo (no dirigido)

        Galaxia desde = g.getGalaxia(actual);
        Galaxia hacia = g.getGalaxia(siguiente);
        string nombreDesde = desde.nombre.empty() ? actual    : desde.nombre;
        string nombreHacia = hacia.nombre.empty() ? siguiente : hacia.nombre;

        cout << setw(20) << nombreDesde << setw(11) << r.id << setw(20) << nombreHacia
             << setw(14) << r.tipo << setw(10) << r.costo << setw(9) << r.tiempo_dias << endl;

        costoTotal  += r.costo;
        tiempoTotal += r.tiempo_dias;
        actual = siguiente;
    }

    cout << string(84, '-') << endl;
    cout << "Costo total: " << costoTotal << "   |   Tiempo total: " << tiempoTotal << " dias" << endl;
}


// ---------------------------------------------------------------------------
// CRUD de VIAJES (historial). Todas reciben el vector<Viaje> global como
// fuente de la verdad y lo modifican por referencia.
// ---------------------------------------------------------------------------

// Devuelve un puntero al viaje con ese id, o nullptr si no existe.
Viaje* buscarViaje (vector<Viaje> &viajes, const string &id) {
    for (Viaje &v : viajes) {
        if (v.id == id) return &v;
    }
    return nullptr;
}

// Agrega un viaje. Falla (false) si ya existe uno con el mismo id.
bool agregarViaje (vector<Viaje> &viajes, const Viaje &nuevo) {
    if (buscarViaje(viajes, nuevo.id) != nullptr) {
        return false;   // id repetido
    }
    viajes.push_back(nuevo);
    return true;
}

// Edita los datos de un viaje existente (identificado por id).
bool editarViaje (vector<Viaje> &viajes, const string &id, string nave_id, string ruta_id,
                  string fecha, float costo_real, bool exitoso,
                  string nave_nombre, string origen_nombre, string destino_nombre) {
    Viaje* v = buscarViaje(viajes, id);
    if (v == nullptr) {
        return false;   // no existe
    }
    v->Editar(nave_id, ruta_id, fecha, costo_real, exitoso, nave_nombre, origen_nombre, destino_nombre);
    return true;
}

// Elimina el viaje con ese id.
bool eliminarViaje (vector<Viaje> &viajes, const string &id) {
    for (int i = 0; i < (int)viajes.size(); i++) {
        if (viajes[i].id == id) {
            viajes.erase(viajes.begin() + i);
            return true;
        }
    }
    return false;   // no existe
}

// Devuelve todos los viajes de una nave (para "historial de viajes por nave").
vector<Viaje> historialDeNave (vector<Viaje> &viajes, const string &nave_id) {
    vector<Viaje> resultado;
    for (const Viaje &v : viajes) {
        if (v.nave_id == nave_id) resultado.push_back(v);
    }
    return resultado;
}

// Copia, dentro de cada nave, los viajes que le corresponden (segun nave_id).
// Usa el vector<Viaje> global como fuente; el historial de cada nave es una
// copia de conveniencia.
void asignarHistorialANaves (vector<Nave> &naves, vector<Viaje> &viajes) {
    for (Nave &n : naves) {
        n.historial = historialDeNave(viajes, n.id);
    }
}


// Busca una nave por id en el vector. Devuelve la nave (copia) si la encuentra,
// o una Nave vacia si no existe.
Nave buscarNave (const vector<Nave> &naves, const string &id) {
    for (const Nave &n : naves) {
        if (n.id == id) {
            return n;
        }
    }
    return Nave();
}


// Imprime todas las naves de un vector en formato tabla.
void imprimirNaves (const vector<Nave> &naves) {
    cout << left;
    cout << setw(10) << "ID" << setw(12) << "CODIGO" << setw(26) << "NOMBRE"
         << setw(12) << "CAPACIDAD" << setw(11) << "VEL.MAX" << setw(8) << "ACTIVA"
         << setw(9) << "VIAJES" << endl;
    cout << string(88, '-') << endl;

    for (const Nave &n : naves) {
        cout << setw(10) << n.id << setw(12) << n.codigo << setw(26) << n.nombre
             << setw(12) << n.capacidad << setw(11) << n.velocidad_Max
             << setw(8) << (n.activa ? "si" : "no")
             << setw(9) << n.historial.size() << endl;
    }
    cout << "Total: " << naves.size() << " naves" << endl;
}


// Imprime, en formato tabla, los viajes que ha hecho una nave (su historial).
void imprimirViajesDeNave (const Nave &nave) {
    cout << "Historial de viajes de " << nave.id;
    if (!nave.nombre.empty()) cout << " (" << nave.nombre << ")";
    cout << ":" << endl;

    if (nave.historial.empty()) {
        cout << "(esta nave no tiene viajes registrados)" << endl;
        return;
    }

    cout << left;
    cout << setw(11) << "VIAJE" << setw(13) << "FECHA" << setw(11) << "RUTA"
         << setw(20) << "ORIGEN" << setw(20) << "DESTINO"
         << setw(11) << "COSTO" << setw(9) << "EXITOSO" << endl;
    cout << string(95, '-') << endl;

    for (const Viaje &v : nave.historial) {
        cout << setw(11) << v.id << setw(13) << v.fecha << setw(11) << v.ruta_id
             << setw(20) << v.origen_nombre << setw(20) << v.destino_nombre
             << setw(11) << v.costo_real << setw(9) << (v.exitoso ? "si" : "no") << endl;
    }
    cout << "Total: " << nave.historial.size() << " viajes" << endl;
}

