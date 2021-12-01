<<<<<<< HEAD
cmake_minimum_required(VERSION 3.20)
project(cv8)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(cv8
        volume/main.cpp
        volume/aabb.h volume/aabb.cpp
        volume/camera.h volume/camera.cpp
        volume/cell.h volume/cell.cpp
        volume/CellHit.h
        volume/intersection.h volume/intersection.cpp
        volume/matrix4x4.h volume/matrix4x4.cpp
        volume/ray.h volume/ray.cpp
        volume/stdafx.h volume/stdafx.cpp
        volume/utils.h
        volume/vector3.h volume/vector3.cpp
        volume/volume.h volume/volume.cpp
        )

target_link_libraries(cv8
        cxxopts
        OpenMP::OpenMP_CXX
        ${OpenCV_LIBS}
=======
cmake_minimum_required(VERSION 3.0.0)
project(VolumetricData VERSION 0.1.0)

find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED)

include_directories(volume)

add_executable(volumetric
        volume/aabb.cpp
        volume/camera.cpp
        volume/cell.cpp
        volume/intersection.cpp
        volume/main.cpp
        volume/matrix4x4.cpp
        volume/ray.cpp
        volume/stdafx.cpp
        volume/vector3.cpp
        volume/volume.cpp
        volume/CellHit.h volume/utils.h)

target_link_libraries(volumetric
        ${OpenCV_LIBS}
        OpenMP::OpenMP_CXX
>>>>>>> volumetric
        )