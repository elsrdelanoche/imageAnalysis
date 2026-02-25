# Mapa de módulos (antes → ahora)

El repositorio original contenía 9 módulos numerados.

Nuevo mapeo en VisionCore SDK:

1) **Color & Intensity** → `Preprocessing` (grayscale, canales, negativo, brillo/contraste)
2) **Image I/O / Basics** → `Preprocessing` (carga, guardado, normalización)
3) **Histogram Operations** → `Preprocessing` (equalización)
4) **Logical Operations** → `Preprocessing` (threshold)
5) **Geometric Transforms** → `Preprocessing` (rotate, scale)
6) **Morphology** → `Preprocessing` (erode/dilate/open/close)
7) **Frequency Transform** → `Frequency` (FFT magnitude/phase)
8) **Filtering & Edges** → `Segmentation & Edges` (gaussian/high-pass/sobel/canny)
9) **Corner Features** → `Feature Extraction` (Harris)
