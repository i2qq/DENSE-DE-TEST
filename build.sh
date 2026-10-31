#!/bin/bash
set -e
echo "Building Dense Language Native Binary..."
gcc -std=c11 -Wall -O2 src/dense_main.c src/dense_eval.c src/dense_dict.c src/dense_proj.c src/dense_de.c src/dense_cb.c src/dense_py.c -o dense
echo "Build complete: ./dense"
