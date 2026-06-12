# Proyecto2_AP2026-2
## Datos

Este proyecto utiliza datos LiDAR del Plan Nacional de Ortofotografía Aérea (PNOA), proporcionados por el Centro Nacional de Información Geográfica (CNIG).

© CNIG - Gobierno de España

Fuente:
https://centrodedescargas.cnig.es/

Especialmente usamos:
- https://centrodedescargas.cnig.es/CentroDescargas/detalleArchivo?sec=12071766

- https://centrodedescargas.cnig.es/CentroDescargas/detalleArchivo?sec=12463756

- https://centrodedescargas.cnig.es/CentroDescargas/detalleArchivo?sec=9404740

- https://centrodedescargas.cnig.es/CentroDescargas/detalleArchivo?sec=11593658


Condiciones de uso:
https://centrodedescargas.cnig.es/CentroDescargas/lidar-tercera-cobertura#licencias

## Compilado y ejecución

Desde la raíz del proyecto se usa `make`. Es requisito tener `mpicc` y `gcc` previamente instalados.

### Versión paralela
Compilar:
```
make par
```
Ejecutar (por defecto 4 procesos):
```
make run-par
```
O especificando el número de procesos:
```
make run-par NP=8
```

### Versión secuencial
Compilar:
```
make seq
```
Ejecutar:
```
make run-seq
```
O especificando el archivo SVG de salida:
```
make run-seq SVG=mi_resultado.svg
```

### Compilar ambas
```
make all
```

### Limpieza
Borrar ejecutables y archivos generados:
```
make clean
```
Borrar solo los archivos intermedios de `work/` (`.pts` y `.tri`):
```
make clean-work
```
Borrar todo incluyendo la carpeta `work/`:
```
make clean-all
```