# VisionCore SDK

Aplicación de escritorio (Dark Mode) para procesamiento de imágenes con **pipeline encadenable** y arquitectura **MVC desacoplada**.

## Objetivo
Unificar los 9 módulos del repositorio original en una sola app profesional, donde:
- El **Modelo** ejecuta el procesamiento pesado (OpenCV / NumPy).
- La **Vista** es moderna y minimalista (Qt).
- El **Controlador** coordina eventos, flujo y encadenamiento entre módulos.

## Módulos funcionales
- **Preprocessing**: transformaciones de intensidad/color, histogramas, lógicas, geométricas y morfología.
- **Frequency**: FFT 2D (magnitud / fase).
- **Segmentation & Edges**: suavizado, realce, Sobel, Canny.
- **Feature Extraction**: Harris corners.

## Ejecución (Arch Linux + fish)
Usa `scripts/visioncore.fish`.

## Estructura
- `src/visioncore/` → SDK (Modelo + pipeline + catálogo de operaciones)
- `src/visioncore_app/` → UI + controladores
- `docs/` → arquitectura y guía de migración
- `tests/` → pruebas unitarias (pytest)

