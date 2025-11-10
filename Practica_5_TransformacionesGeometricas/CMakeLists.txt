cmake_minimum_required(VERSION 3.10)
project(ImageTransform VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Buscar GTKmm3
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

# Incluir directorios
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${GTKMM_INCLUDE_DIRS}
)

# Enlazar directorios
link_directories(${GTKMM_LIBRARY_DIRS})

# Flags de compilación
add_definitions(${GTKMM_CFLAGS_OTHER})

# Archivos fuente
set(SOURCES
    src/main.cpp
    src/model/ImageModel.cpp
    src/view/MainWindow.cpp
    src/controller/TransformController.cpp
    src/utils/ImageProcessor.cpp
)

# Crear ejecutable
add_executable(${PROJECT_NAME} ${SOURCES})

# Enlazar bibliotecas
target_link_libraries(${PROJECT_NAME} ${GTKMM_LIBRARIES})

# Copiar recursos
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
