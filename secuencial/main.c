#include <string.h>
#include "structure.h"
/**
 * Main principal que ejecuta el programa
 *
 * Se le pasa el numero de procesos
 * Cada proceso ejecuta el main (dependiendo el rank):
 * - rank= 0: es el master
 * - rank /= 0 es worker
 */
int main(int argc, char **argv)
{

    // Crear configuracion (archivo de entrada, carpeta de trabajo (.pts y .tri))
    CONFIG cfg;
    strcpy(cfg.input_file, "data/prueba_100.las");
    strcpy(cfg.work_dir, "work");
    strcpy(cfg.svg_out, argc > 1 ? argv[1] : "resultado.svg");
    // Definir tamaño del bloque
    cfg.block_size = 20000;

    // Ejecución secuencial
    sequential_run(&cfg);
    return 0;
}