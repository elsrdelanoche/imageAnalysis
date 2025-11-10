# Morfología Matemática en Imágenes Binarias (GTK3 · C++ · MVC)

GUI para **Erosión, Dilatación, Apertura y Clausura** en **imágenes binarias**, con utilidades de **ruido sal/pimienta** (inyectar y remover). C++17 + gtkmm-3.0 (GTK3), patrón **MVC**.

## Compilar
Arch/Manjaro:
```bash
sudo pacman -S --needed base-devel cmake gtkmm3
mkdir -p build && cd build && cmake .. && cmake --build . -j$(nproc) && ./morph
```
Debian/Ubuntu:
```bash
sudo apt update && sudo apt install -y build-essential cmake libgtkmm-3.0-dev
mkdir -p build && cd build && cmake .. && cmake --build . -j$(nproc) && ./morph
```
Void:
```bash
sudo xbps-install -S cmake gtkmm3-devel
mkdir -p build && cd build && cmake .. && cmake --build . -j$(nproc) && ./morph
```

## Uso
1. **Cargar imagen** y (si hace falta) activar **Forzar binarización (128)**.
2. Elegir **SE** (Cuadrado/Diamante/Disco) y **tamaño impar**.
3. Aplicar **Erosión / Dilatación** o **Apertura / Clausura**.
4. **Sal/Pimienta**: usar probabilidad y botones para agregar/remover
   - Remover **sal** → **Apertura**
   - Remover **pimienta** → **Clausura**
