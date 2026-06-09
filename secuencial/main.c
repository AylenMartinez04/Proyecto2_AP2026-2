#include <string.h>
#include "structure.h"
/**
 * Main principal que ejecuta el programa
 * (Versión secuencial)
 *
 * Todo ocurre en un solo proceso, realizando el split, luego delaunay para cada bloque y merge final.
 *
 * Se mantiene la lógica de división de bloques para una mejor comparación con la versión paralela.
 */
int main(int argc, char **argv)
{

    // Crear configuracion (archivo de entrada, carpeta de trabajo (.pts y .tri))
    CONFIG cfg;
    strcpy(cfg.input_file, "data/PNOA_2021_CAT_462-4603_NPC01.las");
    strcpy(cfg.work_dir, "work");
    strcpy(cfg.svg_out, argc > 1 ? argv[1] : "resultado.svg");
    // Definir tamaño del bloque
    cfg.block_size = 20000;

    // Ejecución secuencial
    sequential_run(&cfg);
    return 0;
}