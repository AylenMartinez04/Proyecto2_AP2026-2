#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "structure.h"
/**
 * Triangulación de Delaunay usando el algoritmo Bowyer-Watson.
 * 
 * Consiste en que dado un conjunto de puntos, los conectamos como triangulos, tal que ningun punto quede dentro
 * del circulo que pasa por esos tres puntos del triangulo. Este algoritmo garantiza triangulos más equilateros.
 * 
 * Los pasos son:
 * 1. Supertrinagulo que contiene TODOS los puntos.
 * 2. Para cada punto nuevo P:
 *    a. Encontrar todos los triángulos cuyo circulo contiene a P.
 *       (son triángulos invalidos porque no cumplen el criterio de Delaunay)
 *    b. Borrar esos triángulos
 *    c. Conectar P con cada arista del hueco. Obtenemos nuevos triángulos.
 * 3. Al final, quitar los triángulos que usan vértices del supertriángulo.
 */

/* Punto en (x,y) ignorando z (que se obtiene de las coordenadas en .las)*/
typedef struct {
    double x, y;
} P2;

/* Definición de triangulo */
typedef struct {
    int a, b, c; // Vertices
    double cx, cy, r2; // centro del circulo (cx,cy) y radio
    int bad; // variable para saber si se elimina
} TRI;

/* Definición de arista*/
typedef struct {
    int a, b;
} EDGE;

/**
 * Función (auxiliar )que mide la distancia de dos puntos y la eleva al cuadrado
 */
double dist2(double x1, double y1, double x2, double y2)
{
    double dx = x1 - x2;
    double dy = y1 - y2;
    return dx*dx + dy*dy;
}

/**
 * Nota: Se le llama circuncírculo al circulo que pasa por tres vertices dados
 * Esta función calcula el circulo que pasa por esos tres puntos
 */
void circumcircle(P2 p1, P2 p2, P2 p3, TRI *t) {
    double d = 2 * (p1.x*(p2.y-p3.y) +
                    p2.x*(p3.y-p1.y) +
                    p3.x*(p1.y-p2.y));

    if (fabs(d) < 1e-12) {
        t->r2 = 1e18;
        return;
    }

    t->cx = ((p1.x*p1.x + p1.y*p1.y)*(p2.y-p3.y) +
             (p2.x*p2.x + p2.y*p2.y)*(p3.y-p1.y) +
             (p3.x*p3.x + p3.y*p3.y)*(p1.y-p2.y)) / d;

    t->cy = ((p1.x*p1.x + p1.y*p1.y)*(p3.x-p2.x) +
             (p2.x*p2.x + p2.y*p2.y)*(p1.x-p3.x) +
             (p3.x*p3.x + p3.y*p3.y)*(p2.x-p1.x)) / d;

    t->r2 = dist2(p1.x, p1.y, t->cx, t->cy);
}

/* Algoritmo de Delaunay */

int delaunay_run(POINT *pts3, int n, const char *out)
{
    printf("[DT] %d puntos\n", n);

    P2 *pts = malloc((n + 3) * sizeof(P2));

    for (int i = 0; i < n; i++) {
        pts[i].x = pts3[i].x;
        pts[i].y = pts3[i].y;
    }

    /* Super triangulo */

    double minx = pts[0].x, maxx = pts[0].x;
    double miny = pts[0].y, maxy = pts[0].y;

    for (int i = 1; i < n; i++) {
        if (pts[i].x < minx) minx = pts[i].x;
        if (pts[i].x > maxx) maxx = pts[i].x;
        if (pts[i].y < miny) miny = pts[i].y;
        if (pts[i].y > maxy) maxy = pts[i].y;
    }

    double dx = maxx - minx;
    double dy = maxy - miny;
    double delta = (dx > dy ? dx : dy) * 10;

    pts[n]   = (P2){minx - delta, miny - delta};
    pts[n+1] = (P2){minx + dx/2,  maxy + delta};
    pts[n+2] = (P2){maxx + delta, miny - delta};

    /* Triangulación */
    int tri_capacity = n * 10;
    TRI *tri = malloc(tri_capacity * sizeof(TRI));    
    int tcount = 1;

    tri[0].a = n;
    tri[0].b = n+1;
    tri[0].c = n+2;
    tri[0].bad = 0;

    circumcircle(pts[n], pts[n+1], pts[n+2], &tri[0]);

    for (int i = 0; i < n; i++) {

        EDGE *edges = malloc(tri_capacity * 3 * sizeof(EDGE));
        int ecount = 0;

        /* Marcar triángulos "malos" */
        for (int j = 0; j < tcount; j++) {
            if (tri[j].bad) continue;

            double d = dist2(pts[i].x, pts[i].y, tri[j].cx, tri[j].cy);

            if (d < tri[j].r2) {
                tri[j].bad = 1;

                edges[ecount++] = (EDGE){tri[j].a, tri[j].b};
                edges[ecount++] = (EDGE){tri[j].b, tri[j].c};
                edges[ecount++] = (EDGE){tri[j].c, tri[j].a};
            }
        }

        /* Eliminar aristas duplicadas */
        for (int j = 0; j < ecount; j++) {
            for (int k = j + 1; k < ecount; k++) {
                if (edges[j].a == edges[k].b && edges[j].b == edges[k].a) {
                    edges[j].a = edges[k].a = -1;
                }
            }
        }

        /* Crear nuevos triángulos */
        for (int j = 0; j < ecount; j++) {
            if (edges[j].a == -1) continue;

            if (tcount >= tri_capacity) {
                tri_capacity *= 2;
                tri = realloc(tri, tri_capacity * sizeof(TRI));
            }

            tri[tcount].a = edges[j].a;
            tri[tcount].b = edges[j].b;
            tri[tcount].c = i;
            tri[tcount].bad = 0;

            circumcircle(
                pts[tri[tcount].a],
                pts[tri[tcount].b],
                pts[i],
                &tri[tcount]
            );

            tcount++;
        }

        free(edges);
    }

    /* Filtrar triangulos */

    FILE *f = fopen(out, "wb");

    int valid = 0;

    for (int i = 0; i < tcount; i++) {
        if (tri[i].bad) continue;
        if (tri[i].a >= n || tri[i].b >= n || tri[i].c >= n) continue;
        valid++;
    }

    fwrite(&valid, sizeof(int), 1, f);

    for (int i = 0; i < tcount; i++) {
        if (tri[i].bad) continue;
        if (tri[i].a >= n || tri[i].b >= n || tri[i].c >= n) continue;

        TRIANGLE t;

        t.x0 = pts3[tri[i].a].x;
        t.y0 = pts3[tri[i].a].y;

        t.x1 = pts3[tri[i].b].x;
        t.y1 = pts3[tri[i].b].y;

        t.x2 = pts3[tri[i].c].x;
        t.y2 = pts3[tri[i].c].y;

        fwrite(&t, sizeof(TRIANGLE), 1, f);
    }

    fclose(f);

    printf("[DT] Triangulos generados: %d\n", valid);

    free(tri);
    free(pts);

    return 0;
}