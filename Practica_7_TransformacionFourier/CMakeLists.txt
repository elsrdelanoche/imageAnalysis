cmake_minimum_required(VERSION 3.10)
project(FourierGTK3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

add_executable(fourier
    src/main.cpp
    src/controller/AppController.cpp
    src/model/ImageModel.cpp
    src/view/MainWindow.cpp
    src/utils/FFT.cpp
    src/utils/ImgUtils.cpp
)

target_include_directories(fourier PRIVATE
    ${GTKMM_INCLUDE_DIRS}
    src
)

target_link_libraries(fourier PRIVATE
    ${GTKMM_LIBRARIES}
)

target_compile_options(fourier PRIVATE ${GTKMM_CFLAGS_OTHER})
