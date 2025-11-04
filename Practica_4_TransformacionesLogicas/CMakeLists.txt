cmake_minimum_required(VERSION 3.10)
project(ImagenesLogicasGTK3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find gtkmm-3.0
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

add_executable(imlogic
    src/main.cpp
    src/controller/AppController.cpp
    src/model/ImageModel.cpp
    src/view/MainWindow.cpp
    src/utils/BinaryOps.cpp
)

target_include_directories(imlogic PRIVATE
    ${GTKMM_INCLUDE_DIRS}
    src
)

target_link_libraries(imlogic PRIVATE
    ${GTKMM_LIBRARIES}
)

target_compile_options(imlogic PRIVATE ${GTKMM_CFLAGS_OTHER})


# Copiar assets al directorio de build para que el binario los encuentre
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
