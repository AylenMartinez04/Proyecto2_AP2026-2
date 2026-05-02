# Proyecto2_AP2026-2
## Datos

Este proyecto utiliza datos LiDAR del Plan Nacional de Ortofotografía Aérea (PNOA), proporcionados por el Centro Nacional de Información Geográfica (CNIG).

© CNIG - Gobierno de España

Fuente:
https://centrodedescargas.cnig.es/

Especialmente usamos, el cual se descarga en el enlace adjunto:
https://centrodedescargas.cnig.es/CentroDescargas/detalleArchivo?sec=12071766

Condiciones de uso:
https://centrodedescargas.cnig.es/CentroDescargas/lidar-tercera-cobertura#licencias

## Compilado y ejecución
Desde la raiz:

Compilar:
`mpicc src/*.c -o triangulation -lm`

Ejecutar:
`mpirun -np n ./triangulation`

Nota: n es el numero de procesos