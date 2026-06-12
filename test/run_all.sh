#!/bin/bash
# run_all.sh
# Corre secuencial + paralelo (NP=4, 8, 12) para cada archivo .las
# Uso: ./run_all.sh

N=5  # número de repeticiones por configuración

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

LAS_FILES=(
    "data/PNOA_2016_MAD_379-4476_ORT-CLA-RGB.las"
    "data/PNOA_2016_MAD_383-4484_ORT-CLA-RGB.las"
    "data/PNOA_2017_NAV_610-4646_ORT-CLA-IRC.las"
    "data/PNOA_2019_CYL-NE_598-4648_000-000-IRC.las"
    "data/PNOA_2021_CAT_462-4603_NPC01.las"
)

for LAS in "${LAS_FILES[@]}"; do
    # Nombre corto del archivo para los logs
    NAME=$(basename "$LAS" .las)

    echo ""
    echo "════════════════════════════════════════════"
    echo " Dataset: $NAME"
    echo "════════════════════════════════════════════"

    echo ">>> Secuencial"
    bash "$SCRIPT_DIR/runtime_sec.sh" $N "resultado_${NAME}_seq.svg" "$LAS"

    echo ">>> Paralelo NP=4"
    bash "$SCRIPT_DIR/runtime_par.sh" $N 4 "$LAS"

    echo ">>> Paralelo NP=8"
    bash "$SCRIPT_DIR/runtime_par.sh" $N 8 "$LAS"

    echo ">>> Paralelo NP=12"
    bash "$SCRIPT_DIR/runtime_par.sh" $N 12 "$LAS"
done

echo ""
echo "════════════════════════════════════════════"
echo " Todas las pruebas terminaron."
echo " Resultados en: $SCRIPT_DIR/results/"
echo "════════════════════════════════════════════"