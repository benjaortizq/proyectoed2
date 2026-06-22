#include "archivos.cpp"   // trae Grafo, GetGrafoURL, fillGalaxiaData, kruskal, dijkstra
#include "raylib.h"
#include <cmath>
#include <set>

using namespace std ;


// Dibuja el grafo en una ventana 2D. Cada galaxia se ubica segun sus
// coordenadas reales (x, y); las aristas son lineas entre galaxias con su costo.
// Si las galaxias no tienen coordenadas utiles (todas en 0), las acomoda en circulo para que igual se vean.
// Las rutas que vengan en "resaltadas" se dibujan en ROJO y mas gruesas (sirve para mostrar un camino de Dijkstra, el arbol de Kruskal, etc.).
void dibujarGrafo (const Grafo &g, const vector<Ruta> &resaltadas) {
    const int ANCHO = 1500;
    const int ALTO  = 800;
    const int MARGEN = 150;   // espacio para que los nodos no toquen el borde

    // Conjunto con los id de las rutas a resaltar, para buscarlas en O(log n).
    set<string> idsResaltadas;
    for (const Ruta &r : resaltadas) {
        idsResaltadas.insert(r.id);
    }

    // 1. Calcular el rango real de coordenadas (bounding box) de las galaxias.
    float minX = 1e9f, maxX = -1e9f, minY = 1e9f, maxY = -1e9f;
    for (const Galaxia &gal : g.galaxias) {
        minX = min(minX, gal.x);  maxX = max(maxX, gal.x);
        minY = min(minY, gal.y);  maxY = max(maxY, gal.y);
    }
    bool sinCoordenadas = (maxX - minX < 1e-3f) || (maxY - minY < 1e-3f);

    // 2. Precalcular la posicion EN PANTALLA de cada galaxia (una sola vez).
    map<string, Vector2> pos;
    if (!sinCoordenadas) {
        for (const Galaxia &gal : g.galaxias) {
            float px = MARGEN + (gal.x - minX) / (maxX - minX) * (ANCHO - 2 * MARGEN);
            // Y invertida: en pantalla crece hacia abajo, queremos +y hacia arriba.
            float py = MARGEN + (maxY - gal.y) / (maxY - minY) * (ALTO - 2 * MARGEN);
            pos[gal.id] = { px, py };
        }
    } else {
        // Sin coordenadas: disponer las galaxias en circulo.
        int n = (int)g.galaxias.size();
        float cx = ANCHO / 2.0f, cy = ALTO / 2.0f;
        float radio = min(ANCHO, ALTO) / 2.0f - MARGEN;
        for (int i = 0; i < n; i++) {
            float ang = 2.0f * PI * i / n;
            pos[g.galaxias[i].id] = { cx + radio * cosf(ang), cy + radio * sinf(ang) };
        }
    }
    InitWindow(ANCHO, ALTO, "Red Galactica - Proyecto ED");
    SetTargetFPS(30);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(Color{ 12, 12, 28, 255 });

        // a. Aristas primero (van detras de los nodos).
        for (const Ruta &r : g.rutas) {
            if (!pos.count(r.origen_id) || !pos.count(r.destino_id)) continue;
            Vector2 a = pos[r.origen_id];
            Vector2 b = pos[r.destino_id];

            // Si la ruta esta en el conjunto a resaltar -> roja y mas gruesa.
            bool resaltada = idsResaltadas.count(r.id) > 0;
            Color  colorLinea = resaltada ? RED : Color{ 90, 90, 120, 255 };
            float  grosor     = resaltada ? 3.5f : 1.5f;
            DrawLineEx(a, b, grosor, colorLinea);

            // costo en el punto medio de la arista
            int mx = (int)((a.x + b.x) / 2.0f);
            int my = (int)((a.y + b.y) / 2.0f);
            Color colorTexto = resaltada ? RED : Color{ 150, 150, 170, 255 };
            DrawText(TextFormat("%.0f", r.costo), mx, my, 11, colorTexto);
        }

        // b. Nodos encima (circulo + nombre).
        for (const Galaxia &gal : g.galaxias) {
            Vector2 p = pos[gal.id];
            DrawCircleV(p, 9, SKYBLUE);
            DrawText(gal.nombre.c_str(), (int)p.x + 11, (int)p.y - 6, 12, RAYWHITE);
        }

        // c. Informacion general.
        DrawText(TextFormat("Galaxias: %d    Rutas: %d",
                            (int)g.galaxias.size(), (int)g.rutas.size()),
                 10, 10, 18, YELLOW);
        DrawText("ESC para salir", 10, ALTO - 24, 14, GRAY);

        EndDrawing();
    }

    CloseWindow();
}


// Dibuja el ARBOL (MST) de Kruskal en una ventana 2D, IGNORANDO las coordenadas
// reales de las galaxias. Genera un layout propio por niveles (BFS): elige una
// raiz, reparte los nodos por profundidad (nivel 0 arriba, hijos debajo) y los
// distribuye horizontalmente. Recibe el grafo que devuelve kruskal().
void GraficarARBOL (const Grafo &arbol) {
    const int ANCHO = 1500;
    const int ALTO  = 800;
    const int MARGEN = 90;

    // 1. Lista de adyacencia del arbol (no dirigido: cada arista va a ambos lados).
    map<string, vector<string>> ady;
    for (const Ruta &r : arbol.rutas) {
        ady[r.origen_id].push_back(r.destino_id);
        ady[r.destino_id].push_back(r.origen_id);
    }

    // Diccionario id -> galaxia, para mostrar el nombre de cada nodo.
    map<string, Galaxia> nodo;
    for (const Galaxia &gal : arbol.galaxias) nodo[gal.id] = gal;

    // 2. BFS para repartir los nodos por niveles. El bucle externo sobre todas
    //    las galaxias hace que tambien funcione si el arbol es un bosque
    //    (varios componentes desconectados): cada uno arranca una nueva raiz.
    map<string, bool> visitado;
    map<string, int>  nivel;
    vector<vector<string>> porNivel;   // porNivel[L] = ids de galaxias en ese nivel

    for (const Galaxia &raiz : arbol.galaxias) {
        if (visitado[raiz.id]) continue;

        visitado[raiz.id] = true;
        nivel[raiz.id] = 0;
        queue<string> cola;
        cola.push(raiz.id);

        while (!cola.empty()) {
            string u = cola.front();
            cola.pop();

            int L = nivel[u];
            if ((int)porNivel.size() <= L) porNivel.resize(L + 1);
            porNivel[L].push_back(u);

            for (const string &v : ady[u]) {
                if (!visitado[v]) {
                    visitado[v] = true;
                    nivel[v] = L + 1;
                    cola.push(v);
                }
            }
        }
    }

    // 3. Calcular la posicion en pantalla de cada nodo segun su nivel.
    map<string, Vector2> pos;
    int niveles = (int)porNivel.size();
    float altoNivel = (niveles <= 1) ? 0 : (float)(ALTO - 2 * MARGEN) / (niveles - 1);

    for (int L = 0; L < niveles; L++) {
        int k = (int)porNivel[L].size();
        for (int i = 0; i < k; i++) {
            float px = MARGEN + (i + 1.0f) / (k + 1.0f) * (ANCHO - 2 * MARGEN);
            float py = (niveles <= 1) ? ALTO / 2.0f : MARGEN + L * altoNivel;
            pos[porNivel[L][i]] = { px, py };
        }
    }

    // 4. Dibujar.
    InitWindow(ANCHO, ALTO, "Arbol de Kruskal (MST) - Proyecto ED");
    SetTargetFPS(30);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(Color{ 12, 12, 28, 255 });

        // a. Aristas del arbol (verdes, gruesas).
        for (const Ruta &r : arbol.rutas) {
            if (!pos.count(r.origen_id) || !pos.count(r.destino_id)) continue;
            Vector2 a = pos[r.origen_id];
            Vector2 b = pos[r.destino_id];
            DrawLineEx(a, b, 2.5f, GREEN);

            int mx = (int)((a.x + b.x) / 2.0f);
            int my = (int)((a.y + b.y) / 2.0f);
            DrawText(TextFormat("%.0f", r.costo), mx, my, 11, Color{ 180, 220, 180, 255 });
        }

        // b. Nodos (circulo + nombre; si no hay nombre, se usa el id).
        for (const Galaxia &gal : arbol.galaxias) {
            if (!pos.count(gal.id)) continue;
            Vector2 p = pos[gal.id];
            DrawCircleV(p, 10, SKYBLUE);
            const string &etiqueta = gal.nombre.empty() ? gal.id : gal.nombre;
            DrawText(etiqueta.c_str(), (int)p.x + 12, (int)p.y - 6, 12, RAYWHITE);
        }

        // c. Info.
        DrawText(TextFormat("Arbol de Kruskal  |  Galaxias: %d   Aristas: %d",
                            (int)arbol.galaxias.size(), (int)arbol.rutas.size()),
                 10, 10, 18, YELLOW);
        DrawText("ESC para salir", 10, ALTO - 24, 14, GRAY);

        EndDrawing();
    }

    CloseWindow();
}



