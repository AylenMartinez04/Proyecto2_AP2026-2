#!/bin/bash
# run_all.sh
# Corre secuencial + paralelo (NP=4, 8, 12) para cada archivo .las
# Uso: ./run_all.sh

N=5  # número de repeticiones por configuración

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

ROOT_DIR="$(dirname "$SCRIPT_DIR")"
DATA_DIR="$ROOT_DIR/data" # Carpeta data del proyecto

echo "════════════════════════════════════════════"
echo " ESCANEANDO CARPETA DE DATOS: $DATA_DIR"
echo "════════════════════════════════════════════"
    
if [ -n "$(ls -A "$DATA_DIR"/*.las 2>/dev/null)" ]; then

    # Pruebas secuenciales
    for LAS in "$DATA_DIR"/*.las
    do
        echo "════════════════════════════════════════════"
        echo " PROCESANDO ARCHIVO: $(basename "$LAS")"
        echo "════════════════════════════════════════════"
        echo ">>> Secuencial"
        bash "$SCRIPT_DIR/runtime_sec.sh" $N "resultado_${NAME}_seq.svg" "$LAS"
        
        echo "Finalizado el análisis para: $(basename "$LAS")"
        echo ""
    done
    # Pruebas paralelas
    for LAS in "$DATA_DIR"/*.las
    do
        echo "════════════════════════════════════════════"
        echo " PROCESANDO ARCHIVO: $(basename "$LAS")"
        echo "════════════════════════════════════════════"
        for np in 4 8 12 16 20
        do
            echo ">>> Ejecutando con NP=$np"
            bash "$SCRIPT_DIR/runtime_par.sh" "$N" "$np" "$LAS"
            sleep 2
        done
        echo "--------------------------------------------"
        echo "Finalizado el análisis para: $(basename "$LAS")"
        echo "════════════════════════════════════════════"
        echo ""
    done
else
    echo "ERROR: No se encontraron archivos .las en la carpeta $DATA_DIR"
    exit 1
fi

echo ""
echo "════════════════════════════════════════════"
echo " Todas las pruebas terminaron."
echo " Resultados en: $SCRIPT_DIR/results/"
echo "════════════════════════════════════════════"
