#!/bin/bash

# Define los directorios a crear
directorios=(
    "include"
    "include/core"
    "include/io"
    "include/model"
    "include/ui"
    "include/ops"
    "src"
    "src/ui"
    "build"
)

# Define los archivos a crear
archivos=(
    "Makefile"
    "include/app.hpp"
    "include/core/pixel_types.hpp"
    "include/core/image_view.hpp"
    "include/core/image_buffer.hpp"
    "include/core/filter.hpp"
    "include/core/pipeline.hpp"
    "include/io/loader.hpp"
    "include/io/exporter.hpp"
    "include/model/adapters.hpp"
    "include/ui/view.hpp"
    "include/ui/controller.hpp"
    "include/ops/brightness_contrast.hpp"
    "include/ops/grayscale.hpp"
    "include/ops/resize.hpp"
    "src/main.cpp"
    "src/ui/controller.cpp"
    "src/ui/view.cpp"
)

# Crea todos los directorios
for dir in "${directorios[@]}"; do
    mkdir -p "$dir"
done

# Crea todos los archivos
for archivo in "${archivos[@]}"; do
    touch "$archivo"
done

echo "¡Estructura de directorios y archivos creada con éxito!"
