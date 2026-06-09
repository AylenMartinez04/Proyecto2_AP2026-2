CC_PAR  = mpicc
CC_SEQ  = gcc
FLAGS   = -lm

PAR_SRC = src/paralelo/*.c
SEQ_SRC = src/secuencial/*.c

PAR_BIN = triangulation_par
SEQ_BIN = triangulation_seq

# Compilación:

all: par seq

par:
	$(CC_PAR) $(PAR_SRC) -o $(PAR_BIN) $(FLAGS)

seq:
	$(CC_SEQ) $(SEQ_SRC) -o $(SEQ_BIN) $(FLAGS)

# Para la ejecución:

# Uso: make run-par NP=4   (por defecto NP=4)
NP ?= 4
run-par: par
	mkdir -p work
	mpirun -np $(NP) ./$(PAR_BIN)

# Uso: make run-seq SVG=resultado.svg   (por defecto resultado.svg)
SVG ?= resultado.svg
run-seq: seq
	mkdir -p work
	./$(SEQ_BIN) $(SVG)

# Limpieza de puntos, triangulación y el archivo svg de visualización además de ejecutables

clean:
	rm -f $(PAR_BIN) $(SEQ_BIN)
	rm -f work/*.pts work/*.tri
	rm -f *.svg

clean-work:
	rm -f work/*.pts work/*.tri

clean-all: clean
	rm -rf work

.PHONY: all par seq run-par run-seq clean clean-work clean-all