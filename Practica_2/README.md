# ColorTransformsGTK (v2)

Transformaciones entre **modelos de color** con **C++ + GTK+ 3**, pensadas para cursos de *Procesamiento de Imágenes / Machine Learning*.
La app muestra, para cada selección, el **pipeline completo Origen → Intermedios → Destino** y permite escoger cómo visualizar cada canal (**Gris / Tintado / Pseudocolor**).


> **Resumen**
> - Panel izquierdo: cargar imagen, vista previa, selector de transformación, **modo de visualización por canal**.
> - Panel derecho: grillas adaptativas con los canales del **origen**, **intermedios** y **destino**, cada uno con su **pie de imagen**.


## ✨ Funcionalidades

- 10 transformaciones entre modelos de color:
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
- **Visualización por canal** (de momento, solo se mostrará *Tintado*):
  - **Tintado**: R/G/B en rojo/verde/azul; C/M/Y en cian/magenta/amarillo; K en gris.

- **UI adaptativa**: miniaturas uniformes; el grid calcula columnas según el ancho.
- **Pipeline completo**: además del destino, se muestran **intermedios útiles** (p.ej., Cmax/Cmin/Δ en HSV; min(R,G,B) en HSI).
- **Arquitectura ligera tipo MVC**:
  - **Modelo**: `transform.*` (fórmulas puras, sin GTK).
  - **Vista**: `ui.*` (widgets/layout y renderizado de canales).
  - **Controlador**: callbacks GTK que conectan ambos.
- **Rango numérico**: internamente normalizamos a `[0,1]`; al visualizar convertimos a bytes **0x00RRGGBB** (0–255).


## 🖥️ Requisitos

### Arch Linux
```bash
sudo pacman -S base-devel cmake pkgconf gtk3 gdk-pixbuf2
```

### Debian/Ubuntu
```bash
sudo apt-get install -y build-essential cmake pkg-config libgtk-3-dev libgdk-pixbuf2.0-dev
```


## ⚙️ Compilación y ejecución

```bash
git clone https://github.com/<tu-usuario>/ColorTransformsGTK_v2.git
cd ColorTransformsGTK_v2
mkdir build && cd build
cmake ..
make -j$(nproc)
./coltrans
```

## 🧭 Uso

1. **Cargar imagen** (PNG/JPG, RGB 8 bits).
2. Elegir la **transformación** en el combo.
3. Escoger **modo de visualización por canal** (de momento, solo *Tintado*).
4. Explorar el panel derecho: secciones **Origen**, **Intermedios**, **Destino**, con pies de imagen.

Sugerencia: cambia el tamaño de la ventana; el grid se refluye para mantener miniaturas uniformes.


## 🧪 Modelos y fórmulas

Todos los valores trabajan normalizados en `[0,1]`. Donde aplica, se recortan a `[0,1]` al volver a RGB.

### RGB ↔ CMY
- **RGB → CMY**: `C=1−R`, `M=1−G`, `Y=1−B`
- **CMY → RGB**: `R=1−C`, `G=1−M`, `B=1−Y`

### CMY ↔ CMYK
- **CMY → CMYK**:
  - `K = min(C,M,Y)`
  - Si `K=1`: `C'=M'=Y'=0`
  - Si no: `C'=(C−K)/(1−K)`, `M'=(M−K)/(1−K)`, `Y'=(Y−K)/(1−K)`
- **CMYK → CMY**:
  - `C = C'*(1−K)+K`, `M = M'*(1−K)+K`, `Y = Y'*(1−K)+K`

### RGB ↔ YIQ
- **RGB → YIQ**:
  [ Y, I, Q ]^T = 
  [[0.299, 0.587, 0.114],
   [0.596, -0.274, -0.322],
   [0.211, -0.523, 0.312]] * [R, G, B]^T
- **YIQ → RGB**:
  [ R, G, B ]^T = 
  [[1.0, 0.956, 0.621],
   [1.0, -0.272, -0.647],
   [1.0, -1.106, 1.703]] * [Y, I, Q]^T
  - **Recortar** R,G,B a `[0,1]`

### RGB ↔ HSI
- **RGB → HSI**:
  - `I = (R+G+B)/3`
  - `S = 1 − min(R,G,B)/I` (si `I>0`)
  - `H` por:
    ```
    θ = arccos( 0.5((R−G)+(R−B)) / sqrt((R−G)^2 + (R−B)(G−B)) )
    H = θ si G≥B, si no H = 2π − θ
    H_normalizado = H / 2π
    ```
- **HSI → RGB**:
  - Tres sectores: `0°–120°`, `120°–240°`, `240°–360°`
  - Fórmulas por sector con `S` e `I` (ver código).

### RGB ↔ HSV
- **RGB → HSV**:
  - `Cmax = max(R,G,B)`, `Cmin = min(R,G,B)`, `Δ = Cmax − Cmin`
  - `H` según componente de `Cmax` y `Δ` (normalizado a `0..1`)
  - `S = 0` si `Cmax=0`; si no `S = Δ / Cmax`
  - `V = Cmax`
- **HSV → RGB**:
  - `C = V*S`, `H' = H*6`, `X = C*(1−| (H' mod 2) − 1 |)`, `m = V−C`
  - Asignar `(R,G,B)` según sextante `H'` y sumarle `m`



## 🧩 Estructura del proyecto

```
ColorTransformsGTK_v2/
├─ CMakeLists.txt
├─ README.md
└─ src/
   ├─ main.cpp              # Arranque de GtkApplication
   ├─ ui.hpp / ui.cpp       # Vista/Controlador: UI, modos, secciones y render
   ├─ image_utils.hpp/.cpp  # Pixbuf↔planos, helpers de visualización (tint/pseudocolor)
   ├─ transform.hpp/.cpp    # Fórmulas puras de los modelos
```



## 📸 Capturas de pantalla

### Transformación RGB -> CMY
<img width="1908" height="1031" alt="image" src="https://github.com/user-attachments/assets/37a1f8b2-bec9-431a-bde1-65a6b2adbc16" />

### Transformación CMY -> RGB
<img width="1908" height="1031" alt="image" src="https://github.com/user-attachments/assets/aa4f9e7e-a818-405e-99c2-80da527613c3" />

### Transformación CMY -> CMYK
<img width="1908" height="1031" alt="image" src="https://github.com/user-attachments/assets/1aad11a6-f0df-4f0a-bee3-4b23dcd0eebc" />

### Transformación CMYK -> CMY
<img width="1908" height="1031" alt="image" src="https://github.com/user-attachments/assets/d8483cd9-8348-4a46-b0d3-3c8e8ad5a9de" />

### Transformación RGB -> YIQ
<img width="1908" height="1031" alt="image" src="https://github.com/user-attachments/assets/2945465e-d9df-4902-a7ea-386f474f06b2" />

### Transformación YIQ -> RGB
<img width="1908" height="1031" alt="image" src="https://github.com/user-attachments/assets/f2c312a8-5c9d-424a-b1c0-5db3f80acd74" />

### Transformación RGB -> HSI
<img width="1908" height="1031" alt="image" src="https://github.com/user-attachments/assets/129de63c-24c9-47ca-a626-4903a171af0e" />

### Transformación HSI -> RGB
<img width="1908" height="1031" alt="image" src="https://github.com/user-attachments/assets/2326c6af-dcfd-4064-ae40-8abf31eabd9c" />

### Transformación RGB -> HSV
<img width="1908" height="1031" alt="image" src="https://github.com/user-attachments/assets/1118f94e-41ce-4eed-baea-02b4ddc386e9" />

### Transformación HSV -> RGB
<img width="1908" height="1031" alt="image" src="https://github.com/user-attachments/assets/857fe146-b318-44b3-931e-dd3d25b48e23" />


## 📜 Licencia
MIT 
