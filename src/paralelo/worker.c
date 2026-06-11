#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "structure.h"
/**
 * Script para un WORKER (rank distinto de 0 (id del proceso))
 * 
 * Su trabajo consiste en:
 * - Esperar mensaje de master
 * - Recibe un bloque
 * - Lee los puntos (del .pts)
 * - Ejecuta Delaunay 
 * - Envia done
 * - Continua trabajando hasta recibir TAG_STOP
 */

 /**
  * Función principal que ejcutan los workers
  */
void worker_loop(int rank) {
    // Worker trabaja hasta que reciba el tag_stop
    while (1) {
        // Worker lee el mensaje recivido
        int msg;
        MPI_Recv(&msg,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        if (msg == TAG_STOP) break; // DETENER WORKER

        // Si recibe el mensaje de task entonces el master le ha asignado un bloque
        if (msg == TAG_TASK) {
            // Worker recive bloque b a trabajar
            BLOCK b;
            MPI_Recv(&b,sizeof(BLOCK),MPI_BYTE,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

            // Abre el archivo correspondiente con los puntos
            FILE *f = fopen(b.pts_file,"rb");

            // Cantidad de puntos
            int n;
            fread(&n,sizeof(int),1,f);

            // Reserva memoria con todos esos puntos
            POINT *pts = malloc(n*sizeof(POINT));
            fread(pts,sizeof(POINT),n,f); // Lee puntos
            fclose(f);

            // Medir tiempo que le toma calcular Delaunay
            double t0 = MPI_Wtime();

            delaunay_run(pts,n,b.tri_file); // Ejecución de delaunay con esos puntos usando Bowter-Watson triangulacion

            double t1 = MPI_Wtime(); // fin del proceso

            // Resultado: worker (id), bloque (id), cuantó tardo (t1-t0), cuantos puntos calculó
            printf("Worker%d bloque %d: %.2f s (%d pts)\n",
                   rank,b.id,t1-t0,n);
            free(pts); // liberar mem

            MPI_Send(&b.id,1,MPI_INT,0,TAG_DONE,MPI_COMM_WORLD); // enviar su resultado al master
        }
    }
}