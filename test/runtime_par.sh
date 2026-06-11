#!/bin/bash
#  runtime_par.sh 
#  Correr con: ./runtime_par.sh <N_repeticiones> <NP>
#       N_repeticiones : cuántas veces se ejecuta el programa
#       NP             : número de procesos (n > 1)
#

set -euo pipefail

# ---------- Argumentos ----------
if [[ $# -lt 2 ]]; then
    echo "Uso: $0 <N_repeticiones> <NP>"
    echo "  N_repeticiones : número de ejecuciones (n > 0)"
    echo "  NP             : número de procesos paralelos (n > 1)"
    exit 1
fi

N="$1"
NP="$2"

if ! [[ "$N" =~ ^[1-9][0-9]*$ ]]; then
    echo "Error: N_repeticiones debe ser un entero positivo."
    exit 1
fi

if ! [[ "$NP" =~ ^[2-9][0-9]*$ ]]; then
    echo "Error: NP debe ser un entero mayor que 1."
    exit 1
fi

# ---------- Paths ----------
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"          # carpeta raíz del proyecto
RESULTS_DIR="$SCRIPT_DIR/results"
TIMESTAMP="$(date +%Y%m%d_%H%M%S)"
LOG_FILE="$RESULTS_DIR/par_NP${NP}_${TIMESTAMP}.txt"

mkdir -p "$RESULTS_DIR"

echo "============================================" | tee "$LOG_FILE"
echo " Tiempo PARALELO" | tee -a "$LOG_FILE"
echo " Número de procesos: $NP" | tee -a "$LOG_FILE"
echo " Número de ejecuciones: $N" | tee -a "$LOG_FILE"
echo "============================================" | tee -a "$LOG_FILE"

# ---------- Ejecuciones ----------
for i in $(seq 1 "$N"); do
    echo -n "  Run $i/$N ... " | tee -a "$LOG_FILE"

    # Ejecutar desde la raíz del proyecto; capturar stdout+stderr
    OUTPUT=$(cd "$ROOT_DIR" && make run-par NP="$NP" 2>&1)

    # Extraer el tiempo de la ÚLTIMA línea: "[TIME] TOTAL: X.X s"
    LAST_LINE=$(echo "$OUTPUT" | tail -n 1)
    TIME_VAL=$(echo "$LAST_LINE" | grep -oE '[0-9]+\.[0-9]+' | head -n 1)

    if [[ -z "$TIME_VAL" ]]; then
        echo "ADVERTENCIA: no se pudo extraer el tiempo de: '$LAST_LINE'" | tee -a "$LOG_FILE"
        TIME_VAL="0"
    fi

    echo "$TIME_VAL s" | tee -a "$LOG_FILE"
    echo "$TIME_VAL" >> "$RESULTS_DIR/par_NP${NP}_${TIMESTAMP}_raw.txt"
done

# ---------- Calcular tiempo promedio ----------
echo "" | tee -a "$LOG_FILE"
echo "--------------------------------------------" | tee -a "$LOG_FILE"

RAW_FILE="$RESULTS_DIR/par_NP${NP}_${TIMESTAMP}_raw.txt"

AVG=$(awk '
    BEGIN { sum=0; count=0 }
    /^[0-9]/ { sum += $1; count++ }
    END {
        if (count > 0)
            printf "%.6f", sum/count
        else
            print "0"
    }
' "$RAW_FILE")

MIN=$(sort -n "$RAW_FILE" | head -n 1)
MAX=$(sort -n "$RAW_FILE" | tail -n 1)

echo " Ejecuciones completadas : $N" | tee -a "$LOG_FILE"
echo " Número de procesos : $NP" | tee -a "$LOG_FILE"
echo " Tiempo promedio  : $AVG s" | tee -a "$LOG_FILE"
echo "============================================" | tee -a "$LOG_FILE"
echo " Log guardado en  : $LOG_FILE"
echo " Raw guardado en  : $RAW_FILE"