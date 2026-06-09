#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "structure.h"
/**
 * Script para leer archivos .las (datos LiDAR) y convetirlos al tipo POINT
 */

 /* Definición del Header de un archivo .las 
 Este orden fue importante para que obtengamos las coordenadas definidas en el archivo
 pues lo leímos en binario y esta fue la versión que coincidia con nuestro .las (version 1.4)
 */
#pragma pack(1)
typedef struct {
    char signature[4];
    uint16_t file_source_id;
    uint16_t global_encoding;
    uint32_t project_id_1;
    uint16_t project_id_2;
    uint16_t project_id_3;
    uint8_t project_id_4[8];
    uint8_t version_major;
    uint8_t version_minor;
    char system_identifier[32];
    char generating_software[32];
    uint16_t creation_day;
    uint16_t creation_year;
    uint16_t header_size;
    uint32_t offset_to_points;
    uint32_t num_vlr;
    uint8_t point_format;
    uint16_t point_length;
    uint32_t legacy_num_points;
    uint32_t legacy_by_return[5];
    double scale_x, scale_y, scale_z;
    double offset_x, offset_y, offset_z;
    double max_x, min_x;
    double max_y, min_y;
    double max_z, min_z;
    uint64_t start_wave;
    uint64_t start_evlr;
    uint32_t num_evlr;
    uint64_t extended_num_points; 
} LASHeader;
#pragma pack()

/**
 * Función implementada para leer un archuvo .las
 * 
 * Return:
 * - pts_out: Arreglo de puntos (nube de puntos dadas por el archivo)
 * - n_out: Cantidad de puntos
 */
int las_read_points(const char *file, POINT **pts_out, int *n_out) {
    // Lectura del archivo
    FILE *f = fopen(file, "rb");
    if (!f) return -1;
    // Leemos el header
    LASHeader hdr;
    // Usamos fread para leer el archivo binario
    // hdr es la direccion de memoria donde guardaremos los datos (nuestra estructura header)
    // usamos sizeof para que lea exactamente todo el archivo
    // solo leemos un elemento de ese tamaño (1)
    // f es el archivo (.las)
    fread(&hdr, sizeof(LASHeader), 1, f);

    // Guardamos el total de puntos (dato por esa parte del header)
    uint64_t total_p = hdr.legacy_num_points;
    if (hdr.version_major == 1 && hdr.version_minor >= 4 && total_p == 0) {
        total_p = hdr.extended_num_points;
    }

    int n = (int)total_p; // n almacena el total de puntos
    printf("LEYENDO: %d puntos\n", n);

    POINT *pts = malloc(n * sizeof(POINT));// Areglo para guardar los puntos

    fseek(f, hdr.offset_to_points, SEEK_SET); // Buscamos los puntos con fseek, en el archivo

    for (int i = 0; i < n; i++) { // leer cada punto (los obtenemos por el header offset_to_points)
        // cada coordenada en el archivo esta en 32 bits = 4bytes
        int32_t x, y, z; // Definimos coordenadas
        // Leemos con fread (4 bytes) y guardaremos en &x, &y, &z, solo guardamos un elemento
        fread(&x, 4, 1, f);
        fread(&y, 4, 1, f);
        fread(&z, 4, 1, f);
        // Convertimos a coordenadas reales (cada punto en el arreglo de POINTs)
        // x_real = (x scale_x) + ofsset_x
        pts[i].x = x * hdr.scale_x + hdr.offset_x;
        pts[i].y = y * hdr.scale_y + hdr.offset_y;
        pts[i].z = z * hdr.scale_z + hdr.offset_z;
        // Saltar la intensidad y esos datos 
        fseek(f, hdr.point_length - 12, SEEK_CUR);
    }
    fclose(f);
    // Devolver
    *pts_out = pts;
    *n_out = n;
    return 0;
}