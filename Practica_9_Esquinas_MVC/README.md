# Práctica 09 — Extracción de esquinas (MVC) en Python

GUI (Tkinter) para:
- Convolución con **Kirsch** (8 máscaras) y **Frei-Chen** (9 máscaras) y visualización del mapa de respuesta.
- Detector de esquinas **Harris–Stephens** (R = det(M) − k·trace(M)^2) con umbral y **non-maximum suppression**.
- Cargar **dos imágenes** (ej. la imagen del punto 1 y la *antena*) y aplicar los mismos métodos para comparar.

## Requisitos
- Python 3.10+ (incluye Tkinter en la mayoría de instalaciones)
- Dependencias:
  ```bash
  pip install -r requirements.txt
  ```

## Ejecutar
```bash
python main.py
```

## Uso rápido
1) **File → Open Image A/B** para cargar la imagen y la antena.  
2) Selecciona **Active Image** (A o B).  
3) Elige método: **Kirsch**, **Frei‑Chen**, **Harris**.  
4) Ajusta parámetros (si aplica) y da **Apply** (o **Apply to A + B** para procesar las dos).  
5) En la derecha tienes pestañas: *Original*, *Response*, *Overlay* (cuando aplica).

## Notas de teoría
- **Kirsch**: calcula la respuesta en 8 direcciones y toma el máximo (por defecto usamos `max(abs(response))` para no depender del signo del borde).
- **Frei‑Chen**: usa 9 máscaras ortogonales y estima la “energía” proyectada al subespacio de bordes (W1..W4) vs la energía total (W1..W9).
- **Harris**: las esquinas son puntos donde la variación de intensidad es alta en *ambas* direcciones (autovalores grandes del tensor de estructura). Se seleccionan picos locales (NMS).

### Si en Linux te falta Tkinter
En distros mínimas a veces no viene instalado:

- Debian/Ubuntu:
  ```bash
  sudo apt-get install python3-tk
  ```
- Arch:
  ```bash
  sudo pacman -S tk
  ```
- Fedora:
  ```bash
  sudo dnf install python3-tkinter
  ```

### Exportaciones automáticas 
Cada vez que aplica un método, el programa guarda resultados en:
`<carpeta_de_la_imagen>/outputs/<A|B>_<metodo>/`

- Kirsch: `kirsch_combined.png` y (opcional) `kirsch_N.png`, `kirsch_NE.png`, etc.
- Frei‑Chen: `frei_chen_edge_strength.png` y `frei_W1.png` … `frei_W9.png`
- Harris: `harris_R.png`, `harris_overlay.png`, `harris_points_xy.csv`

## Estructura (MVC)
- `app/model/image_ops.py`  → kernels + convolución + Harris
- `app/view/app_view.py`    → UI (Tkinter)
- `app/controller/app_controller.py` → eventos, parámetros, render
- `main.py`                 → arranque

