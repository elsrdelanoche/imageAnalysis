
# Herramientas de Análisis de Imágenes  
**Proyecto integrador de Prácticas 1 a 8**

Este proyecto unifica todas las prácticas de procesamiento digital de imágenes en una sola aplicación profesional basada en **GTKmm**, con soporte para múltiples imágenes, manipulación visual, panel dinámico de parámetros y una arquitectura Modelo‑Vista‑Controlador.

---

## 📦 Características principales

### 🖼️ Área de trabajo tipo “canvas”
- Cargar múltiples imágenes.
- Moverlas libremente con el mouse.
- Seleccionar la imagen activa (la última clickeada).
- Menú contextual:
  - **Guardar imagen como…**
  - **Eliminar imagen**
- Cada operación genera una nueva imagen encima de la activa.

---

## 🧰 Barra de menús (organizada por práctica)

### **Archivo**
- Abrir imagen
- Salir

---

## **P1 – Transformaciones de Color**
- Escala de grises  
- Extracción de canales (R, G, B)  
- Negativo  
- Brillo y contraste (con sliders)

---

## **P2–3 – Histogramas**
- Ecualización de histograma en escala de grises

---

## **P4 – Operaciones Lógicas**
- Umbral binario ajustable

---

## **P5 – Transformaciones Geométricas**
- Rotación (ángulo configurable)
- Escalado (sx, sy) con interpolación bilineal

---

## **P6 – Morfología Matemática**
- Erosión  
- Dilatación  
- Apertura  
- Cierre  
- Tamaño del SE ajustable (3–15, impar)

---

## **P7 – Transformada de Fourier**
- FFT 2D (imagen reescalada a potencia de 2)
- Visualización:
  - Espectro de **magnitud**
  - Espectro de **fase**

---

## **P8 – Filtrado y Detección de Bordes**
- Pasa bajas Gaussiano (k, sigma)
- High‑pass (α)
- Bordes por gradiente (Sobel)
- Detector de Canny (k, sigma, t_low, t_high)

---

## 🧩 Arquitectura (MVC)

```
AppWindow        → Ventana principal / menús
AppController    → Dispara las operaciones
LeftPanel        → Panel dinámico para parámetros
CanvasArea       → Manejo de imágenes, movimiento, contexto
operations.cpp   → Integra todos los algoritmos
utils/           → Código refactorizado de todas las prácticas
```

---

## ⚙️ Compilación

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
./herramientas_analisis_imagenes
```

---

## 🚀 Dependencias
- GTKmm 3  
- Cairo  
- C++17  
- CMake 3.10+  

---

## 📄 Licencia
Uso académico libre.
