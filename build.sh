#!/usr/bin/env bash
mkdir -p bin
g++ -std=c++20 -O2 src/*.cpp -Iinclude -o bin/raytraced-frontal-area
if [ $? -eq 0 ]; then echo "Build succeeded"; else echo "Build failed"; fi
