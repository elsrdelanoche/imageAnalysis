# ColorTransformsGTK (v2)

Aplicación GUI en C++ (GTK+ 3) para transformaciones entre **modelos de color** mostrando **origen → intermedios → destino** con **pies de imagen** por canal y **modos de visualización** (Gris / Tintado / Pseudocolor).

## Novedades
- Panel izquierdo: vista previa, selector de transformación y **panel de Acciones/ Cálculos** con las fórmulas aplicadas.
- Panel derecho: secciones **Origen**, **Intermedios**, **Destino** con miniaturas **uniformes y adaptativas**. Cada tile incluye pie de imagen con canal y rol.
- Modo por canal: **Gris**, **Tintado** (R,G,B,C,M,Y) u **Pseudocolor**. Para **H** se usa color real de tono (o pseudocolor si lo eliges).

## Transformaciones soportadas
1. RGB → CMY
2. CMY → RGB
3. CMY → CMYK
4. CMYK → CMY
5. RGB → YIQ
6. YIQ → RGB
7. RGB → HSI
8. HSI → RGB
9. RGB → HSV
10. HSV → RGB

## Requisitos (Arch Linux)
```bash
sudo pacman -S base-devel cmake pkgconf gtk3 gdk-pixbuf2
```

## Compilar
```bash
cd ColorTransformsGTK_v2
mkdir build && cd build
cmake ..
make -j$(nproc)
./coltrans
```
