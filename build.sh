#!/usr/bin/bash

set -e

mkdir -p build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug -B build -S .
cmake --build build --target particle_sim -- -j $(nproc)
