#include "menu.cpp"
int main () {

    Grafo G ; 

    GetGrafoURL (G, ENDPOINT) ; 
    G.print () ;
    fillGalaxiaData(G) ;
    dibujarGrafo (G) ;
    return 0 ;

}