# Transformaciones en el dominio de la frecuencia (GTK3 · C++ · MVC)

Aplicación para **DFT/FFT 2D** (directa e inversa) sobre imágenes **cuadradas** con **tamaño potencia de dos**.
Muestra **magnitud (log)** y **fase**, con opción de **centrar el espectro (shift de cuadrantes)**.

## Compilar
Arch/Manjaro
```bash
sudo pacman -S --needed base-devel cmake gtkmm3
mkdir -p build && cd build && cmake .. && cmake --build . -j"$(nproc)"
./fourier
```
Debian/Ubuntu
```bash
sudo apt update && sudo apt install -y build-essential cmake libgtkmm-3.0-dev
mkdir -p build && cd build && cmake .. && cmake --build . -j"$(nproc)"
./fourier
```

## Uso
1) **Cargar imagen** (se convierte a escala de grises).  
2) La app **ajusta automáticamente** a **N×N** con **N potencia de dos** via *re-escalado bilinear*.  
3) **FFT**: muestra espectro de **magnitud log** y **fase**; opción **Shift** para centrar bajas frecuencias.  
4) **IFFT**: reconstruye imagen espacial desde espectro actual.  
5) **Guardar** resultado.

> Nota: La FFT 2D se calcula por separabilidad: FFT 1D en filas y luego columnas.
