cmake_minimum_required(VERSION 3.10)
project(FiltrosBordesGTK3 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
add_executable(filtros
    src/main.cpp
    src/controller/AppController.cpp
    src/model/ImageModel.cpp
    src/view/MainWindow.cpp
    src/utils/ImgUtils.cpp
    src/utils/Convolution.cpp
    src/utils/Kernels.cpp
    src/utils/Canny.cpp
)
target_include_directories(filtros PRIVATE ${GTKMM_INCLUDE_DIRS} src)
target_link_libraries(filtros PRIVATE ${GTKMM_LIBRARIES})
target_compile_options(filtros PRIVATE ${GTKMM_CFLAGS_OTHER})
