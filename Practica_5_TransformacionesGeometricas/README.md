# ImageTransform - Transformaciones Geométricas de Imágenes

Aplicación de escritorio para aplicar transformaciones geométricas a imágenes digitales.

## Instalación de Dependencias

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install -y pkg-config build-essential cmake libgtkmm-3.0-dev
```

### Arch Linux
```bash
sudo pacman -S pkg-config base-devel cmake gtkmm3
```

## Compilación

```bash
mkdir -p build && cd build
cmake ..
make -j$(nproc)
./ImageTransform
```

## Características

- **Traslación**: Desplazamiento en píxeles (Tx, Ty)
- **Rotación**: Rotación con ángulo en grados
- **Escalamiento**: Cambio de tamaño con factores (Sx, Sy)
- **Interpolación**: Vecino cercano, Bilineal, Bicúbica

## Uso

1. Abrir imagen (Archivo → Abrir)
2. Ajustar parámetros en panel derecho
3. Aplicar transformación
4. Guardar resultado (Archivo → Guardar)

## Arquitectura MVC

```
src/
├── model/          # ImageModel - Gestión de datos
├── view/           # MainWindow - Interfaz GTKmm3
├── controller/     # TransformController - Lógica
└── utils/          # ImageProcessor - Algoritmos
```
