
# 📊 Práctica: Histograma (GTK+3 + C++17) — MVC

- Panel **izquierdo**: ancho fijo (340 px). El recuadro de **imagen de entrada** se fija a 300×220; cualquier imagen cargada se **ajusta** a ese tamaño preservando aspecto.
- Panel **derecho**: grid con **2 contenedores por fila** y **barra de desplazamiento vertical**. Todas las vistas tienen tamaño uniforme **560×320** e incluyen **ejes X–Y con valores** (Cairo + Pango).

## Salidas (en orden)
1) **Histograma** `h[i]` (0..255)  
2) **φ[i]** — “Histograma phi de la imagen de entrada”  
3) **CDF** — “Densidad Dpi de la imagen entrapa”  
4) **Propiedades estadísticas**: min, max, moda, media, mediana, var, σ, skew, kurt, entropía (bits)  
5) **Ecualización** por CDF: imagen + nueva φ[i]  
6) **Transformaciones** Tabla 2.1 (Negativo, Log, Gamma, Estiramiento lineal 2–98%): imagen + histograma

## Compilar
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/practica_histograma
```
<img width="1908" height="1023" alt="image" src="https://github.com/user-attachments/assets/bf6f9ed8-0b88-4151-ac3c-20a450576cbb" />

<img width="1908" height="1023" alt="image" src="https://github.com/user-attachments/assets/8adaef75-9c0a-4026-8e98-e346b60b9535" />

