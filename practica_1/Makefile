# Makefile para Image Channel Extractor

# Compilador y flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Paquetes GTK - Versión compatible con diferentes shells
GTK_CFLAGS := $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS := $(shell pkg-config --libs gtk+-3.0)

# Archivos
TARGET = image_channel_extractor
SOURCES = main.cpp

# Reglas
all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(GTK_CFLAGS) -o $(TARGET) $(SOURCES) $(GTK_LIBS)

# Instalación de dependencias
deps-ubuntu:
	sudo apt update
	sudo apt install -y build-essential pkg-config libgtk-3-dev

deps-arch:
	sudo pacman -S base-devel pkgconf gtk3 harfbuzz

deps: deps-ubuntu

# Ejecutar
run: $(TARGET)
	./$(TARGET)

# Limpiar
clean:
	rm -f $(TARGET)

# Debug
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

.PHONY: all clean run deps debug
