#!/bin/bash

echo "======================================"
echo "  ImageTransform - Compilación"
echo "======================================"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${YELLOW}Verificando dependencias...${NC}"
if ! pkg-config --exists gtkmm-3.0; then
    echo -e "${RED}Error: gtkmm-3.0 no encontrado${NC}"
    echo "Instale: sudo apt-get install libgtkmm-3.0-dev"
    exit 1
fi

mkdir -p build
cd build

echo -e "${YELLOW}Limpiando...${NC}"
rm -rf *

echo -e "${YELLOW}Configurando CMake...${NC}"
if ! cmake ..; then
    echo -e "${RED}Error en CMake${NC}"
    exit 1
fi

echo -e "${YELLOW}Compilando...${NC}"
if ! make -j$(nproc); then
    echo -e "${RED}Error en compilación${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Compilación exitosa${NC}"
echo ""
echo "Ejecutar: ./build/ImageTransform"
echo ""

read -p "¿Ejecutar ahora? (s/n): " -n 1 -r
echo
if [[ $REPLY =~ ^[SsYy]$ ]]; then
    ./ImageTransform
fi
