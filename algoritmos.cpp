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

    // 1-2. Cada nodo arranca en su propio conjunto.
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
    //    Como es no dirigido, cada ruta se agrega a sus DOS extremos.
    map<string, vector<Ruta>> adyacencia;
    for (const Ruta &r : g.rutas) {
        adyacencia[r.origen_id].push_back(r);
        adyacencia[r.destino_id].push_back(r);
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

