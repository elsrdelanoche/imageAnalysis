cmake_minimum_required(VERSION 3.10)
project(HerramientasAnalisisImagenes LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

# Recopila todos los .cpp del proyecto
file(GLOB SRC_CPP
    "src/*.cpp"
    "src/utils/*.cpp"
)

add_executable(herramientas_analisis_imagenes ${SRC_CPP})

target_include_directories(herramientas_analisis_imagenes PRIVATE
    ${GTKMM_INCLUDE_DIRS}
    src
)

target_link_libraries(herramientas_analisis_imagenes PRIVATE
    ${GTKMM_LIBRARIES}
)

target_compile_options(herramientas_analisis_imagenes PRIVATE
    ${GTKMM_CFLAGS_OTHER}
)
