cmake_minimum_required(VERSION 3.10)
project(ColorTransformsGTK LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GDKPIXBUF REQUIRED gdk-pixbuf-2.0)

add_executable(coltrans
    src/main.cpp
    src/ui.cpp
    src/transform.cpp
    src/image_utils.cpp
)

target_include_directories(coltrans PRIVATE
    ${GTK3_INCLUDE_DIRS}
    ${GDKPIXBUF_INCLUDE_DIRS}
    src
)

target_link_libraries(coltrans
    ${GTK3_LIBRARIES}
    ${GDKPIXBUF_LIBRARIES}
    m
)

add_definitions(${GTK3_CFLAGS_OTHER})
