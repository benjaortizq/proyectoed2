#include "menu.cpp"
int main () {

    Grafo G ; 

    GetGrafoURL (G, ENDPOINT) ;
    G.print () ;
    fillGalaxiaData(G) ;

    // Rutas a resaltar en rojo. Vacio = no se resalta nada.
    // Ej: para resaltar el arbol de Kruskal -> kruskal(G).rutas
    //     para resaltar un camino de Dijkstra -> dijkstra(G, "galaxia-1", "galaxia-9")
    vector<Ruta> resaltadas ;

    dibujarGrafo (G, resaltadas) ;
    return 0 ;

}