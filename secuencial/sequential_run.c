#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "structure.h"

/**
 * Script principal de la versión secuencial.
 * Reemplaza a master.c y worker.c
 *
 * En la versión paralela master dividía el trabajajo y enviaba bloques a los workers usando MPI, cada worker recibiía un bloque,
 * leía sus puntos y ejecutaba Delaunay.
 * Esta versión secuencial hace todo en un solo proceso.
 *
 * 1. Split del trabajo en bloques.
 * 2. Por cada bloque leer puntos, luego ejecutrar Delaunay
 * 3. Al final un merge de todos los resultados
 *
 * Además se mide el tiempo de ejecución total.
 */
int sequential_run(const CONFIG *cfg)
{
    // Parte 1: Split
    clock_t t0 = clock(); // Función para medir el tiempo total

    // Hacer split
    BLOCK *blocks;                // Arreglo de bloques (cada bloque es asignado como un trabajo a un worker)
    int nb;                       // Numero de bloques
    split_run(cfg, &blocks, &nb); // Función que nos devuelve lo anterior

    // Tomamos el tiempo que toma hacer el split calculando:
    double t_split = (double)(clock() - t0) / CLOCKS_PER_SEC;
    printf("[TIME] Split: %.2f s\n", t_split);

    // Parte 2: Procesar cada bloque (leer puntos y ejecutar Delaunay)
    for (int i = 0; i < nb; i++)
    {
        // Bloque de trabajo a procesar en esta iteración
        BLOCK b = blocks[i];

        // Abre el archivo correspondiente con los puntos
        FILE *f = fopen(b.pts_file, "rb");

        // Cantidad de puntos
        int n;
        fread(&n, sizeof(int), 1, f);

        // Reserva memoria con todos esos puntos
        POINT *pts = malloc(n * sizeof(POINT));
        fread(pts, sizeof(POINT), n, f); // Lee puntos
        fclose(f);

        // Ejecutar Delaunay y medir el tiempo del la triangulación para el bloque
        clock_t tb0 = clock();
        delaunay_run(pts, n, b.tri_file); // Ejecución de delaunay con esos puntos usando Bowter-Watson triangulacion
        double tb1 = (double)(clock() - tb0) / CLOCKS_PER_SEC;

        // Resultado: bloque (id), cuantó tardo (t1-t0), cuantos puntos calculó
        printf("Bloque %d: %.2f s (%d pts)\n", b.id, tb1, n);
        free(pts);
    }

    double t_proc = (double)(clock() - t0) / CLOCKS_PER_SEC;
    printf("[TIME] Proc secuencial: %.2f s\n", t_proc);

    /* --- MERGE --- */
    clock_t t2 = clock();
    char final_tri[256];
    merge_all(blocks, nb, final_tri);

    double t_merge = (double)(clock() - t2) / CLOCKS_PER_SEC;
    printf("[TIME] Merge: %.2f s\n", t_merge);

    /* Leer todos los puntos para el SVG (necesitamos la nube completa) */
    POINT *pts = NULL;
    int n = 0;
    las_read_points(cfg->input_file, &pts, &n);

    /* Generar SVG con el resultado final */
    write_svg(cfg->svg_out, pts, n, final_tri);
    free(pts);

    double t_total = (double)(clock() - t0) / CLOCKS_PER_SEC;
    printf("[TIME] TOTAL: %.2f s\n", t_total);
    return 0;
}