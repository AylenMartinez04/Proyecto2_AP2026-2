#include <mpi.h>
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
int main(int argc, char **argv) {
    MPI_Init(&argc,&argv); // Iniciar MPI
    // Obtener rank (ID)
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    // Crear configuracion (archivo de entrada, carpeta de trabajo (.pts y .tri))
    CONFIG cfg;
    strcpy(cfg.input_file,"data/PNOA_2016_MAD_379-4476_ORT-CLA-RGB.las");
    strcpy(cfg.work_dir,"work");
    strcpy(cfg.svg_out,    argc > 1 ? argv[1] : "resultado.svg");
    // Definir tamaño del bloque
    cfg.block_size = 20000;
    // Dependiendo del rank ejecutara su rol
    if(rank==0)
        master_run(&cfg);
    else
        worker_loop(rank);
    MPI_Finalize(); // Finalizar proceso paralelizable
    return 0;
}