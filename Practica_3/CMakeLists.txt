
cmake_minimum_required(VERSION 3.10)
project(Practica_Histograma LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0 gdk-pixbuf-2.0 pango pangocairo)

add_executable(practica_histograma
    src/main.cpp
    src/ui/view.cpp
    src/ui/controller.cpp
)

target_include_directories(practica_histograma PRIVATE include ${GTK3_INCLUDE_DIRS})
target_link_libraries(practica_histograma ${GTK3_LIBRARIES})
target_compile_options(practica_histograma PRIVATE ${GTK3_CFLAGS_OTHER})
