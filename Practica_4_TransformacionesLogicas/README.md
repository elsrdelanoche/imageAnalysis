# Proyecto: Operaciones Lógicas y Relacionales entre Imágenes (GTK3 · C++ · MVC)

**Autor:** Alfredo  
**Materia:** Procesamiento Digital de Imágenes  
**Lenguaje:** C++17  
**Interfaz:** GTK3 (gtkmm 3.24)  
**Arquitectura:** Modelo–Vista–Controlador (MVC)  
**Versión:** 1.0.4

---

## 🧠 Descripción General

Este proyecto implementa una aplicación de escritorio que permite **procesar dos imágenes** (A y B) aplicando:

- **Operaciones lógicas:** `AND`, `OR`, `XOR`, con opciones de `NOT A` y `NOT B`.
- **Operaciones relacionales:** `==`, `!=`, `>`, `>=`, `<`, `<=`.

La interfaz está desarrollada en **C++ con GTK3 (gtkmm)** y se estructura bajo el patrón **MVC** para mantener el código modular, mantenible y extensible.

El programa está optimizado para trabajar con **imágenes binarias (0/255)**, aunque puede convertir automáticamente imágenes en escala de grises a binario mediante la opción **“Forzar binarización (umbral=128)”**.

---

## 🧩 Estructura del Proyecto

```
ImagenesLogicasGTK3/
├── CMakeLists.txt
├── README.md
├── assets/
│   └── ui.glade                # Interfaz gráfica diseñada con GtkBuilder
└── src/
    ├── controller/
    │   ├── AppController.cpp   # Lógica de eventos y flujo principal
    │   └── AppController.hpp
    ├── model/
    │   ├── ImageModel.cpp      # Carga, binarización y operaciones sobre imágenes
    │   └── ImageModel.hpp
    ├── utils/
    │   ├── BinaryOps.cpp       # Operaciones lógicas y relacionales en pixeles
    │   ├── BinaryOps.hpp
    │   └── Types.hpp           # Enumeraciones de operaciones
    ├── view/
    │   ├── MainWindow.cpp      # Vista principal (interfaz Gtk)
    │   └── MainWindow.hpp
    └── main.cpp
```

---

## ⚙️ Requisitos

### Linux (Arch, Ubuntu, Void)

- **CMake ≥ 3.10**
- **C++17 compatible (g++ 9 o superior)**
- **gtkmm-3.0**

Instalación por distribución:

#### Arch / Manjaro
```bash
sudo pacman -S --needed base-devel cmake gtkmm3
```

#### Debian / Ubuntu
```bash
sudo apt update
sudo apt install -y build-essential cmake libgtkmm-3.0-dev
```

#### Void Linux
```bash
sudo xbps-install -S cmake gtkmm3-devel
```

---

## 🧱 Compilación

```bash
unzip ImagenesLogicasGTK3_v4.zip
cd ImagenesLogicasGTK3
mkdir -p build && cd build
cmake ..
cmake --build . -j"$(nproc)"
./imlogic
```

> 🔧 El proceso de compilación copia automáticamente la carpeta `assets/` dentro del directorio `build/` para garantizar que `ui.glade` sea localizado correctamente.

---

## 🖥️ Uso del Programa

1. **Cargar Imagen A** y **Cargar Imagen B** desde la barra lateral izquierda.  
2. (Opcional) Activar **Forzar binarización (umbral=128)** para convertir a blanco/negro.  
3. Seleccionar una **operación lógica** (AND/OR/XOR) o **relacional** (==, !=, >, >=, <, <=).  
4. Pulsar **Aplicar** → el resultado se mostrará a la derecha.  
5. Pulsar **Guardar resultado** para exportar el resultado como `PNG`.

---

## 🧮 Implementación Técnica

### Lógica

- Las imágenes se procesan mediante **Gdk::Pixbuf** (buffers RGB de 8 bits).  
- Si el número de canales es >1, se convierte a escala de grises (luma).  
- Si la opción de binarización está activa, se aplica el umbral:  
  ```
  pixel >= 128 ? 255 : 0
  ```
- Las operaciones lógicas se realizan **bit a bit** sobre los valores binarios.  
- Las operaciones relacionales devuelven una nueva imagen máscara (0 o 255).

### Arquitectura MVC

- **Model (ImageModel):** Carga, preprocesamiento y operaciones.  
- **View (MainWindow):** Interfaz gráfica, manejo de eventos Gtk.  
- **Controller (AppController):** Conecta vista y modelo, gestiona la interacción del usuario.  

---

## 🧾 Licencia

Este proyecto se distribuye bajo la licencia **MIT**.  
Se permite el uso educativo, modificación y redistribución con atribución al autor original.

---

## 🧠 Créditos

- **Autor:** Alfredo  
- **Asesor académico:** Profesor de Procesamiento Digital de Imágenes  
- **Institución:** ESCOM - Instituto Politécnico Nacional  
- **Desarrollo y documentación:** 2025
