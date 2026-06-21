#include "algoritmos.cpp"   // trae Grafo, GetGrafoURL, fillGalaxiaData, kruskal, dijkstra
#include "raylib.h"
#include <cmath>

using namespace std ;


// Dibuja el grafo en una ventana 2D. Cada galaxia se ubica segun sus
// coordenadas reales (x, y); las aristas son lineas entre galaxias con su costo.
// Si las galaxias no tienen coordenadas utiles (todas en 0), las acomoda en
// circulo para que igual se vean.
void dibujarGrafo (const Grafo &g) {
    const int ANCHO = 1100;
    const int ALTO  = 800;
    const int MARGEN = 70;   // espacio para que los nodos no toquen el borde

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

    // 3. Abrir la ventana y dibujar en bucle hasta que cierren (ESC o la X).
    InitWindow(ANCHO, ALTO, "Red Galactica - Proyecto ED");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(Color{ 12, 12, 28, 255 });

        // a. Aristas primero (van detras de los nodos).
        for (const Ruta &r : g.rutas) {
            if (!pos.count(r.origen_id) || !pos.count(r.destino_id)) continue;
            Vector2 a = pos[r.origen_id];
            Vector2 b = pos[r.destino_id];
            DrawLineEx(a, b, 1.5f, Color{ 90, 90, 120, 255 });

            // costo en el punto medio de la arista
            int mx = (int)((a.x + b.x) / 2.0f);
            int my = (int)((a.y + b.y) / 2.0f);
            DrawText(TextFormat("%.0f", r.costo), mx, my, 11, Color{ 150, 150, 170, 255 });
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



