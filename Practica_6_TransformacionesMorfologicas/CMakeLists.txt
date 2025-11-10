cmake_minimum_required(VERSION 3.10)
project(MorfologiaGTK3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

add_executable(morph
    src/main.cpp
    src/controller/AppController.cpp
    src/model/ImageModel.cpp
    src/view/MainWindow.cpp
    src/utils/MorphOps.cpp
    src/utils/SEFactory.cpp
    src/utils/Noise.cpp
)

target_include_directories(morph PRIVATE
    ${GTKMM_INCLUDE_DIRS}
    src
)

target_link_libraries(morph PRIVATE
    ${GTKMM_LIBRARIES}
)

target_compile_options(morph PRIVATE ${GTKMM_CFLAGS_OTHER})

# Copy assets to build dir
# assets no longer needed
