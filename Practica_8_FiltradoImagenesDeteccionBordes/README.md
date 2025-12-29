# Filtros y Detección de Bordes (GTK3 · C++17 · MVC)

Aplicación de procesamiento digital de imágenes implementada en **C++17** con interfaz **GTKmm3**, bajo arquitectura **Modelo–Vista–Controlador (MVC)**.  
El sistema permite **filtrar y detectar bordes** mediante convolución general y operadores clásicos de primer y segundo orden, incluyendo el algoritmo **Canny**.

---

## 🎯 Objetivo

Aplicar técnicas de filtrado espacial y detección de contornos sobre imágenes digitales, implementando desde cero:
- **Convolución 2D** con tratamiento de bordes mediante *reflect padding*.
- **Filtros pasa-bajas y pasa-altas**.
- **Operadores de detección de bordes** de primer y segundo orden.
- **Detector de Canny**, combinando suavizado, gradiente, supresión no máxima y umbralización con histéresis.

---

## 🧩 Arquitectura del proyecto

```
FiltrosBordesGTK3/
 ├── src/
 │   ├── controller/ → Controlador principal (AppController)
 │   ├── model/      → Lógica y procesamiento de imágenes (ImageModel)
 │   ├── utils/      → Herramientas: convolución, kernels, Canny, conversión
 │   └── view/       → Interfaz gráfica (MainWindow)
 ├── CMakeLists.txt  → Configuración de compilación
 └── README.md
```

---

## 🧱 Dependencias

- **C++17**
- **GTKmm 3.0**
- **CMake ≥ 3.10**

### En Arch/Manjaro
```bash
sudo pacman -S --needed base-devel cmake gtkmm3
```

### En Debian/Ubuntu
```bash
sudo apt update
sudo apt install -y build-essential cmake libgtkmm-3.0-dev
```

---

## ⚙️ Compilación y ejecución

```bash
unzip FiltrosBordesGTK3.zip -d FiltrosBordesGTK3
cd FiltrosBordesGTK3
mkdir build && cd build
cmake ..
cmake --build . -j"$(nproc)"
./filtros
```

---

## 🖥️ Uso de la aplicación

1. **Cargar imagen:** Se convierte internamente a escala de grises.  
2. **Seleccionar categoría y kernel:**
   - *Pasa-bajas:* Media 3×3 / 5×5 / 7×7, **Media k×k** (usa *k (impar)*), Gaussiano (k y σ configurables)
   - *Pasa-altas:* Sharpen, Laplaciano (4 / 8 vecinos), High-boost ((1 + α) I − LP)
   - *Bordes:* Roberts, Prewitt, Sobel, Scharr, Kirsch, Robinson, **LoG (usa k (impar) y σ)**
3. **Ajustar parámetros:** tamaño de kernel, sigma, α (High-boost), etc.
4. **Aplicar filtro:** se muestra la imagen resultante.
5. **Ejecutar Canny:** con k, σ, umbral bajo y alto configurables.
6. **Guardar resultado:** exporta la imagen procesada en formato PNG.

---

## 🧮 Principales algoritmos implementados

- **Convolución 2D:**
  \\[
  g(x,y) = \sum_i \sum_j f(x-i, y-j) \, h(i,j)
  \\]
  con relleno reflectante para evitar pérdida de bordes.

- **Pasa-bajas:** suavizado mediante máscara media y Gaussiana.
- **Pasa-altas:** realce de bordes (Sharpen, Laplaciano, High-boost).
- **Detección de bordes:** operadores de gradiente (Roberts, Prewitt, Sobel, Scharr) y compás (Kirsch, Robinson).
- **Canny:**
  1. Suavizado gaussiano  
  2. Cálculo de gradiente (Sobel)  
  3. Supresión no máxima (NMS)  
  4. Doble umbral  
  5. Conexión por histéresis

---

## 📊 Detalles técnicos

- Representación interna en **grises normalizados [0,1]**
- Visualización auto-escalada (min-max) para derivadas
- **Reflect padding** (evita bordes oscuros)
- Kernels configurables en tamaño impar (3 – 31)
- Soporte para ampliaciones futuras (p. ej., *LoG 9×9* o *DoG)
