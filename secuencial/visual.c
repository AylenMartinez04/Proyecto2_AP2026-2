#include <stdio.h>
#include <stdlib.h>
#include "structure.h"

/**
 * Lee el archivo .tri binario producido por merge_all y genera un SVG.
 *
 * Args:
 *  - svg_path : ruta de salida del SVG
 *  - pts      : nube de puntos original (para dibujar los vértices)
 *  - n        : cantidad de puntos
 *  - tri_bin  : archivo .tri binario (formato: int32 count, luego N TRIANGLE)
 */

#define SVG_W  1200
#define SVG_H   900
#define MARGIN   60

void write_svg(const char *svg_path, POINT *pts, int n, const char *tri_bin)
{
    FILE *fb = fopen(tri_bin, "rb");
    if (!fb) { fprintf(stderr, "[SVG] No se pudo abrir %s\n", tri_bin); return; }

    int tcount;
    fread(&tcount, sizeof(int), 1, fb);

    typedef struct { double x0,y0, x1,y1, x2,y2; } TRI6;
    TRI6 *tris = malloc(tcount * sizeof(TRI6));
    for (int i = 0; i < tcount; i++) {
        TRIANGLE t;
        fread(&t, sizeof(TRIANGLE), 1, fb);
        tris[i] = (TRI6){t.x0,t.y0, t.x1,t.y1, t.x2,t.y2};
    }
    fclose(fb);

    /* Bounding box de los puntos originales */
    double minx = pts[0].x, maxx = pts[0].x;
    double miny = pts[0].y, maxy = pts[0].y;
    for (int i = 1; i < n; i++) {
        if (pts[i].x < minx) minx = pts[i].x;
        if (pts[i].x > maxx) maxx = pts[i].x;
        if (pts[i].y < miny) miny = pts[i].y;
        if (pts[i].y > maxy) maxy = pts[i].y;
    }

    double world_w = maxx - minx;
    double world_h = maxy - miny;
    double draw_w  = SVG_W - 2*MARGIN;
    double draw_h  = SVG_H - 2*MARGIN;
    double sx = draw_w / world_w;
    double sy = draw_h / world_h;
    double s  = (sx < sy) ? sx : sy;   /* escala uniforme, sin deformar */

    double off_x = MARGIN + (draw_w - world_w * s) / 2.0;
    double off_y = MARGIN + (draw_h - world_h * s) / 2.0;

    #define TX(x)  (off_x + ((x) - minx) * s)
    #define TY(y)  (SVG_H - off_y - ((y) - miny) * s)   /* Y invertido */

    FILE *f = fopen(svg_path, "w");
    if (!f) { free(tris); return; }

    fprintf(f,
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<svg xmlns=\"http://www.w3.org/2000/svg\""
        " width=\"%d\" height=\"%d\" viewBox=\"0 0 %d %d\">\n",
        SVG_W, SVG_H, SVG_W, SVG_H);

    /* Fondo */
    fprintf(f, "  <rect width=\"%d\" height=\"%d\" fill=\"#0f1117\"/>\n", SVG_W, SVG_H);

    /* Título */
    fprintf(f,
        "  <text x=\"%d\" y=\"32\" font-family=\"monospace\" font-size=\"15\""
        " fill=\"#a0aec0\" text-anchor=\"middle\">"
        "Delaunay (MPI) — %d puntos · %d triángulos</text>\n",
        SVG_W/2, n, tcount);

    /* Coordenadas reales */
    fprintf(f,
        "  <text x=\"%d\" y=\"%d\" font-family=\"monospace\" font-size=\"10\""
        " fill=\"#4a5568\" text-anchor=\"middle\">"
        "X [%.1f , %.1f]   Y [%.1f , %.1f]</text>\n",
        SVG_W/2, SVG_H - 14, minx, maxx, miny, maxy);

    /* Triángulos */
    fprintf(f, "  <g id=\"triangles\">\n");
    for (int i = 0; i < tcount; i++) {
        int hue = (i * 137) % 360;
        fprintf(f,
            "    <polygon points=\"%.2f,%.2f %.2f,%.2f %.2f,%.2f\""
            " fill=\"hsla(%d,50%%,25%%,0.4)\""
            " stroke=\"#4fd1c5\" stroke-width=\"0.6\" stroke-linejoin=\"round\"/>\n",
            TX(tris[i].x0), TY(tris[i].y0),
            TX(tris[i].x1), TY(tris[i].y1),
            TX(tris[i].x2), TY(tris[i].y2),
            hue);
    }
    fprintf(f, "  </g>\n");

    double r = (n > 10000) ? 0.8 : (n > 2000) ? 1.2 : (n > 500) ? 1.8 : 2.5;
    fprintf(f, "  <g id=\"points\">\n");
    for (int i = 0; i < n; i++) {
        fprintf(f,
            "    <circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.1f\""
            " fill=\"#f6e05e\" stroke=\"none\"/>\n",
            TX(pts[i].x), TY(pts[i].y), r);
    }
    fprintf(f, "  </g>\n");

    fprintf(f, "</svg>\n");
    fclose(f);
    free(tris);

    printf("[SVG] %s  (%d pts, %d tri)\n", svg_path, n, tcount);

    #undef TX
    #undef TY
}