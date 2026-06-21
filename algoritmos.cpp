#include "API.cpp"
#include <map>

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
