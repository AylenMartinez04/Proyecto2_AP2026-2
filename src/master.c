#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "structure.h"
/**
 * Script para ejecutar el MASTER (rank 0 (id del proceso))
 * 
 * Su trabajo es:
 * - Dividir el trabajo (hacer SPLIT)
 * - Mandar/Asignar bloques a los workes
 * - Cuando los workers terminan, el master asigna más (si hay).
 * - Cuando se han terminado los bloques, el proceso termina.
 * - Master obtiene el resultado final con el merge
 * 
 * En este script además se mide el tiempo de ejecución
 */

 /**
  * Función principal que solo la ejecuturá el proceso con rank=0
  */
int master_run(const CONFIG *cfg) {
    double t0 = MPI_Wtime(); // Función para medir el tiempo total

    // Hacer split
    BLOCK *blocks; // Arreglo de bloques (cada bloque es asignado como un trabajo a un worker)
    int nb; // Numero de bloques
    split_run(cfg,&blocks,&nb); // Función que nos devuelve lo anterior 

    // Tomamos el tiempo que toma hacer el split calculando:
    double t_split = MPI_Wtime() - t0;
    printf("[TIME] Split: %.2f s\n", t_split);

    int size; // Numero de procesos
    MPI_Comm_size(MPI_COMM_WORLD,&size); // ¿Cuantos procesos estoy ejecutando en total?

    int next=0; // Siguiente bloque a enviar
    int done=0; // Cantidad de bloques terminados

    double t1 = MPI_Wtime(); // Medir el tiempo de trabajo (procesamiento de bloques)
    // EL master va a asignar un bloque a cada worker disponible (inicialmente)
    for(int i=1;i<size && next<nb;i++){
        int msg=TAG_TASK;
        // dato a enviar, cantidad de datos enviados, tipo de dato, a que proceso va, tag, comunicador
        MPI_Send(&msg,1,MPI_INT,i,0,MPI_COMM_WORLD); // Se envía mensaje (tipo de mensaje)
        MPI_Send(&blocks[next],sizeof(BLOCK),MPI_BYTE,i,0,MPI_COMM_WORLD); // Se envia mensaje (bloque asignado)
        next++; 
    }
    // Mientras falten bloques a procesar
    while(done<nb){
        MPI_Status status;
        int id;
        // El master recibe un mensaje del primer worker que temrina
        MPI_Recv(&id, 1, MPI_INT, MPI_ANY_SOURCE, TAG_DONE, MPI_COMM_WORLD, &status);
        int worker = status.MPI_SOURCE;
        done++;
        // Worker checa si aun hay bloques por asignar, y si si se la asgina al worker disponible
        if(next<nb){
            int msg=TAG_TASK;
            MPI_Send(&msg, 1, MPI_INT, worker, 0, MPI_COMM_WORLD);
            MPI_Send(&blocks[next], sizeof(BLOCK), MPI_BYTE, worker, 0, MPI_COMM_WORLD);
            next++;
        }
    }

    double t_proc = MPI_Wtime() - t1;
    printf("[TIME] MPI Proc: %.2f s\n", t_proc);

    /* Stop workers */
    // Una vez que next = done saldrá del while, entonces, se han procesado todos los bloques
    // el master avisa a los trabajadores
    for(int i=1;i<size;i++){
        int msg=TAG_STOP;
        MPI_Send(&msg,1,MPI_INT,i,0,MPI_COMM_WORLD); // Mensaje de stop
    }

    /* Merge */
    double t2 = MPI_Wtime();

    merge_all(blocks, nb);

    double t_merge = MPI_Wtime() - t2;
    printf("[TIME] MERGE: %.2f s\n", t_merge);


    double t_total = MPI_Wtime() - t0;
    printf("[TIME] TOTAL: %.2f s\n", t_total);
    return 0;
}