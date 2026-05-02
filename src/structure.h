#ifndef STRUCTURE_H
#define STRUCTURE_H
#define TAG_TASK 1 // Etiqueta de mensaje cuando master le envía mensaje a un worker
#define TAG_DONE 2 // Etiqueta de mensaje cuando worker termina tarea
#define TAG_STOP 3 // Etiqueta de mensaje cuando el master envía señal de detener la ejecución o que ha terminado
#include <mpi.h>
/*
 * Definiciones que son compartidas en varios archivos
 */

/* Definición de un punto (x,y,z) */
typedef struct {
    double x, y, z; // Coordenadas .las (nube de puntos)
} POINT;

/* Definición de triángulo (para la triangulación) */
typedef struct {
    double x0, y0, z0; // Vertice A
    double x1, y1, z1; // Vertice B
    double x2, y2, z2; // Vertice C
} TRIANGLE;

/* Bloque (grid) */
typedef struct {
    int id; // Identificador
    char pts_file[256]; // Archivo con puntos de ese bloque (.pts)
    char tri_file[256]; // Archivo donde se guardan (.tri)
} BLOCK;

/* Parte de la configuracion */
typedef struct {
    char input_file[256]; // Archivo .las
    char work_dir[256]; // Archivo donde se guarda (.pts y .tri)
    int block_size; // // Tamaño del bloque
} CONFIG;

/**
 * Función para leer los puntos del .las
 * 
 * Args: 
 * - file: archivo .las
 * - pts: arreglo de puntos (apunta directo a un dato Point)
 * - n: cantidadad
 */
int las_read_points(const char *file, POINT **pts, int *n);

/**
 * Divide los puntos de un bloque
 * 
 * Args:
 * - cfg: configuración (.las, salida, tamaño bloque, tamaño bloque)
 * - blocks: divide los puntos en un arreglo de bloques
 * - num_bloques: numero de bloques
 */
int split_run(const CONFIG *cfg, BLOCK **blocks, int *num_blocks);

/**
 * Uner los resultados por niveles
 */
int merge_all(BLOCK *blocks, int nb);

/**
 * Une los resultados de la triangulación
 */
int merge_run(const CONFIG *cfg, BLOCK *blocks, int nb);

/**
 * Aplica la trinagulación de Delaunay
 * 
 * Args:
 * - pts: puntos
 * - n: n triaungulos
 * - outfile: archuvo de salida
 */
int delaunay_run(POINT *pts, int n, const char *out_file);

/**
 * Funcion de cada worker
 */
void worker_loop(int rank);

/**
 * Funcion del master
 * 
 * Nota: recibe cfg porque es el que tomará el archivo .las y lo va a dividir en bloques
 */
int master_run(const CONFIG *cfg);

#endif