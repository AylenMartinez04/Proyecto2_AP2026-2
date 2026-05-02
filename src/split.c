#include <stdio.h>
#include <stdlib.h>
#include "structure.h"
/**
 * Scipt que se encaragará de dividir la nube de puntos en bloques (grids).
 * Cada bloque tiene una coordenada (x,y) asociada.
 */

 /**
  * Función principal: SPLIT
  * 
  * El objetivo es que el conjunto de datos se descomponga en bloques mas pequeños
  * para procesarse de forma independiente y simultanea (paralelamente).
  * 
  * Se sigue el pipeline:
  * - Leer archivo .las
  * - Dividir (split) nube de puntos en bloques
  * - Obtenemos bloques de puntos, estos bloques se almacenan en /work/.pts
  * - Esos bloques se le pasan a los workes
  */
int split_run(const CONFIG *cfg, BLOCK **blocks_out, int *num_blocks) {
    // Leer
    POINT *pts; // Arreglo de puntos
    int n; // Cantidad de puntos
    las_read_points(cfg->input_file, &pts, &n);

    // Definir el tamaño de los bloque (se define pensando en cuantos puntos hace cada bloque)
    int size = cfg->block_size; // Se define en el main en la estructura de configuración
    int nb = (n + size - 1) / size; // Calculo de CUANTOS bloques tenemos.

    // Reservar memoria para los bloques
    BLOCK *blocks = malloc(nb * sizeof(BLOCK));

    for (int i = 0; i < nb; i++) { // Iteramos sobre cada bloque
        // Repartimos los puntos con la formula:
        int start = i * size;
        int end = start + size;
        if (end > n) end = n;

        // Creamos los archivos .pts y .tri (/data)
        sprintf(blocks[i].pts_file, "%s/block_%d.pts", cfg->work_dir, i);
        sprintf(blocks[i].tri_file, "%s/block_%d.tri", cfg->work_dir, i);

        // Abrimos archivo de puntos .pts
        FILE *f = fopen(blocks[i].pts_file, "wb");
        if (!f) {
            printf("Error creando archivo: %s\n", blocks[i].pts_file);
            exit(1);
        }
        int count = end - start; // Numero de puntos que hay en el bloque
        fwrite(&count, sizeof(int), 1, f); // Damos los puntos correspondientes a cada archivo .pts
        fwrite(&pts[start], sizeof(POINT), count, f);
        fclose(f);

        blocks[i].id = i; // Guardamos id (numero de bloque)
    }

    free(pts); // Liberamos memoria de pts
    // Guardamos los cambios regresando: bloques y el numero de bloques
    *blocks_out = blocks;
    *num_blocks = nb;
    return 0;
}