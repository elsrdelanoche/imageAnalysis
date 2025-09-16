# 🎨 Image Channel Extractor (GTK + C++17)

Aplicación de escritorio en **C++17** con **GTK+3** para la **lectura, separación y manipulación de imágenes**.  
Permite visualizar los canales RGB de una imagen, generar una versión en escala de grises y aplicar controles de **brillo** y **contraste** en tiempo real. Además, se pueden exportar los datos de la imagen en formato **CSV** para análisis posterior.

---

## ✨ Características

- 📂 **Carga de imágenes** (`.jpg`, `.jpeg`, `.png`, `.bmp`, `.gif`).
- 🔴🟢🔵 **Separación de canales**:
  - Canal **Rojo**
  - Canal **Verde**
  - Canal **Azul**
- ⚫ **Conversión a escala de grises** (promedio simple, extensible a BT.601/BT.709).
- 🌗 **Ajuste dinámico de brillo y contraste**:
  - Se aplican tanto a la imagen original como a la escala de grises.
- 📊 **Exportación a CSV**:
  - Formatos: `0x00RRGGBB`, Rojo, Verde, Azul, Gris (0–255).
- 🖥️ **Interfaz gráfica con GTK** (Arch Linux friendly).
- 🧩 Arquitectura **Modelo–Vista–Controlador (MVC)**, escalable y mantenible.

---

## 🗂️ Estructura del proyecto

```
.
├── include/
│   ├── app.hpp
│   ├── core/              # Tipos base y pipeline
│   ├── io/                # Carga/exportación
│   ├── model/             # Adaptadores GdkPixbuf <-> core
│   ├── ops/               # Operaciones de imagen (filtros)
│   └── ui/                # Vista GTK + Controlador
├── src/
│   ├── main.cpp           # Punto de entrada
│   ├── ui/
│   │   ├── view.cpp
│   │   └── controller.cpp
├── Makefile
└── README.md
```

---

## ⚙️ Instalación y compilación (Arch Linux)

1. Instalar dependencias:

```bash
sudo pacman -S base-devel gcc make pkgconf gtk3 gdk-pixbuf2
```

2. Clonar el repositorio y compilar:

```bash
git clone https://github.com/tuusuario/image-channel-extractor.git
cd image-channel-extractor
make
```

3. Ejecutar:

```bash
./image_channel_extractor
```

---

## 🖱️ Uso

1. Abrir la aplicación y seleccionar una imagen desde el **Menú**.
2. Visualizar:
   - Imagen original.
   - Canales R, G, B.
   - Escala de grises.
3. Ajustar **Brillo** y **Contraste** mediante los sliders.
4. Exportar la imagen procesada como **CSV** eligiendo el modo desde el combo.

---

## 🏗️ Arquitectura

El proyecto sigue un enfoque **MVC**:

- **Modelo (`core/`, `ops/`, `io/`, `model/`)**  
  Lógica de procesamiento de imagen y conversión de formatos.
- **Vista (`ui/view.*`)**  
  Interfaz gráfica con GTK (renderizado de imágenes, sliders, botones).
- **Controlador (`ui/controller.*`)**  
  Conecta la Vista con el Modelo, maneja eventos y aplica filtros.

---

## 📸 Capturas

*(Agrega aquí imágenes de la ventana principal mostrando los canales y sliders)*

---

## 📚 Referencias

- Documentación GTK: [https://www.gtk.org/docs/](https://www.gtk.org/docs/)  
- GdkPixbuf Reference: [https://docs.gtk.org/gdk-pixbuf/](https://docs.gtk.org/gdk-pixbuf/)  
- Fórmulas de luminancia (BT.601 / BT.709)  

---

## 🧑‍💻 Autor

Proyecto desarrollado en **C++17** como práctica universitaria de **Procesamiento Digital de Imágenes**.  
Diseñado y probado en **Arch Linux**.  

---

## 📜 Licencia

MIT License © 2025  
