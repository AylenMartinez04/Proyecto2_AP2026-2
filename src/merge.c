#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structure.h"
/**
 * Script MERGE
 * 
 * Sigue el paradigma n-level binary tree
 */
int merge_two(const char *f1, const char *f2, const char *out) {
    // Abre dos archivos de respuesta por los workers
    FILE *a = fopen(f1, "rb");
    FILE *b = fopen(f2, "rb");
    // Archivo de la union (salida)
    FILE *o = fopen(out, "wb");

    if (!a || !b || !o) {
        printf("Error en merge_two\n");
        exit(1);
    }
    // Tamaño de los datos
    int n1, n2;
    fread(&n1, sizeof(int), 1, a);
    fread(&n2, sizeof(int), 1, b);
    // Total de datos a unir
    int total = n1 + n2;
    fwrite(&total, sizeof(int), 1, o); // memoria para unir los resultados

    TRIANGLE *buf = malloc(sizeof(TRIANGLE));

    for (int i = 0; i < n1; i++) {
        fread(buf, sizeof(TRIANGLE), 1, a);
        fwrite(buf, sizeof(TRIANGLE), 1, o);
    }

    for (int i = 0; i < n2; i++) {
        fread(buf, sizeof(TRIANGLE), 1, b);
        fwrite(buf, sizeof(TRIANGLE), 1, o);
    }

    free(buf);
    fclose(a);
    fclose(b);
    fclose(o);

    return 0;
}

/* Merge binario por niveles */
int merge_all(BLOCK *blocks, int nb)
{
    char files[2000][256];

    /* Inicializar lista con archivos originales */
    for (int i = 0; i < nb; i++) {
        strcpy(files[i], blocks[i].tri_file);
    }

    int current = nb;
    int level = 0;

    while (current > 1) {
        int next = 0;

        for (int i = 0; i < current; i += 2) {

            if (i + 1 < current) {
                char out[256];
                sprintf(out, "work/merge_L%d_%d.tri", level, next);

                merge_two(files[i], files[i+1], out);

                strcpy(files[next], out);
            } else {
                /* Caso impar */
                strcpy(files[next], files[i]);
            }

            next++;
        }

        current = next;
        level++;
    }

    printf("[MERGE] Final: %s\n", files[0]);
    return 0;
}