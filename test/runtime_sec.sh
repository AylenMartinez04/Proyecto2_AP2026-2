#!/bin/bash
#  runtime_sec.sh 
#  Correr con: ./runtime_seq.sh <N_repeticiones> [SVG]
#       N_repeticiones : cuántas veces se ejecuta el programa

set -euo pipefail

# ---------- Argumentos ----------
if [[ $# -lt 1 ]]; then
    echo "Uso: $0 <N_repeticiones> [SVG]"
    echo "  N_repeticiones : número de ejecuciones (entero > 0)"
    echo "  SVG            : archivo de salida SVG (default: resultado.svg)"
    exit 1
fi

N="$1"
SVG="${2:-resultado.svg}"

if ! [[ "$N" =~ ^[1-9][0-9]*$ ]]; then
    echo "Error: N_repeticiones debe ser un entero positivo."
    exit 1
fi

# ---------- Paths ----------
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
RESULTS_DIR="$SCRIPT_DIR/results"
TIMESTAMP="$(date +%Y%m%d_%H%M%S)"
LOG_FILE="$RESULTS_DIR/seq_${TIMESTAMP}.txt"
RAW_FILE="$RESULTS_DIR/seq_${TIMESTAMP}_raw.txt"

mkdir -p "$RESULTS_DIR"

# ---------- Encabezado ----------
echo "============================================" | tee "$LOG_FILE"
echo " Tiempo SECUENCIAL"                        | tee -a "$LOG_FILE"
echo " Numero de ejecuciones: $N"                               | tee -a "$LOG_FILE"
echo "============================================" | tee -a "$LOG_FILE"

# ---------- Ejecuciones ----------
for i in $(seq 1 "$N"); do
    echo -n "  Run $i/$N ... " | tee -a "$LOG_FILE"

    OUTPUT=$(cd "$ROOT_DIR" && make run-seq SVG="$SVG" 2>&1)

    LAST_LINE=$(echo "$OUTPUT" | tail -n 1)
    TIME_VAL=$(echo "$LAST_LINE" | grep -oE '[0-9]+\.[0-9]+' | head -n 1)

    if [[ -z "$TIME_VAL" ]]; then
        echo "ADVERTENCIA: no se pudo extraer el tiempo de: '$LAST_LINE'" | tee -a "$LOG_FILE"
        TIME_VAL="0"
    fi

    echo "$TIME_VAL s" | tee -a "$LOG_FILE"
    echo "$TIME_VAL"   >> "$RAW_FILE"
done

# ---------- Calcular estadísticas ----------
echo ""                                             | tee -a "$LOG_FILE"
echo "--------------------------------------------" | tee -a "$LOG_FILE"

AVG=$(awk 'BEGIN{s=0;c=0} /^[0-9]/{s+=$1;c++} END{if(c>0) printf "%.6f",s/c; else print "0"}' "$RAW_FILE")
MIN=$(sort -n "$RAW_FILE" | head -n 1)
MAX=$(sort -n "$RAW_FILE" | tail -n 1)

echo " Ejecuciones completadas : $N"    | tee -a "$LOG_FILE"
echo " SVG              : $SVG"  | tee -a "$LOG_FILE"
echo " Tiempo promedio  : $AVG s" | tee -a "$LOG_FILE"
echo "============================================" | tee -a "$LOG_FILE"
echo " Log guardado en  : $LOG_FILE"
echo " Raw guardado en  : $RAW_FILE"