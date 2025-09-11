# 🖼️ Extractor de Canales RGB (GTK/C++)

App de escritorio en C++/GTK para:

* 🌈 **Separar canales** R, G y B
* ⚫ **Escala de grises**
* 💡 **Brillo** y 🎚️ **Contraste** (en la imagen original y en la de grises)
* 📤 **Exportar CSV** (Color `0x00RRGGBB`, Rojo, Verde, Azul, Gris)

> La arquitectura “lector con getters + vista” y la separación por **máscaras/corrimientos** está inspirada en los materiales de clase. &#x20;
<img width="1908" height="1031" alt="image" src="https://github.com/user-attachments/assets/19325b1a-5613-45ce-9d08-9ebf84168ca3" />
<img width="1908" height="1031" alt="image" src="https://github.com/user-attachments/assets/07d7d857-5348-4784-bfb4-48bf09adb6d0" />
<img width="1908" height="1031" alt="image" src="https://github.com/user-attachments/assets/a43cc062-0ece-4be6-b431-aeec45913650" />


---

## 🐧 Enfoque Arch Linux

### 📦 Dependencias (Arch/Manjaro)

```bash
sudo pacman -S --needed base-devel gtk3 gdk-pixbuf2 glib2 pango cairo pkgconf
```

> Si usas AUR helpers: `yay -S` para paquetes adicionales si te faltara alguno.

### 🏗️ Compilar

Con el `Makefile` incluido:

```bash
make clean && make
```

O manualmente con `pkg-config`:

```bash
g++ -std=c++17 -Wall -Wextra main.cpp -o image_channel_extractor \
  $(pkg-config --cflags --libs gtk+-3.0 gdk-pixbuf-2.0)
```

### ▶️ Ejecutar

```bash
./image_channel_extractor
```

---

## ✨ Uso rápido

1. En el panel **Menú**:

   * 📁 **Seleccionar Imagen** (`.jpg/.jpeg/.png/.bmp/.gif`)
   * 👁️ Se muestra el **nombre de archivo** debajo del ícono
   * Ajusta **Brillo** y **Contraste** → verás cambios en **Imagen Original** y **Grises**
2. 📤 **Exportar CSV**

   * Elige **Modo para exportar a CSV**:

     * **Color (0x00RRGGBB)**, **Rojo**, **Verde**, **Azul**, **Gris**
   * Guarda una **matriz 2D** (una fila por renglón de la imagen escalada)

---

## 🔧 Detalles de implementación

* **Separación RGB** vía máscaras/corrimientos:

  * `R = (pix & 0x00ff0000) >> 16`
  * `G = (pix & 0x0000ff00) >> 8`
  * `B = (pix & 0x000000ff)` &#x20;
* **Grises**: promedio simple `(R + G + B) / 3`
* **Brillo/Contraste**:

  * Fórmula por píxel: `v' = clamp((v + brillo) * contraste, 0, 255)`
  * Sin normalización min–max (para que los sliders se sientan “reales”)

---

## 🧪 API interna (estilo Java)

Para replicar ejercicios/labs:

* `getImagenInt(mode) -> std::vector<std::vector<uint32_t>>`

  * `COLOR_RGB_PACKED` → `0x00RRGGBB` (mapeo tipo `BufferedImage` int)
  * `RED_8U / GREEN_8U / BLUE_8U / GRAY_8U` → 0..255
* `convertirInt2DA1D(matriz2D) -> std::vector<uint32_t>` (row-major)

> Patrón “lector con getters” + conversión a vector, como en el material de clase.&#x20;

---

## 🗂️ Estructura

```
.
├── Makefile
├── README.md
└── main.cpp  # UI + procesamiento + exportación CSV + API tipo Java
```


## 📝 Licencia
**MIT**
