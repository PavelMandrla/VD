cmake_minimum_required(VERSION 3.20)
project(cv9)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)
find_package(VTK REQUIRED)

include(${VTK_USE_FILE})

add_executable(cv9 main.cpp
        Grid.cpp Grid.h
        Particle.h
        GridPoint.cpp GridPoint.h
        defines.h
        GridCell.cpp GridCell.h
        defines.cpp)

target_link_libraries(cv9
        OpenMP::OpenMP_CXX
        ${VTK_LIBRARIES}
        )

add_executable(test test.cpp)

target_link_libraries(test
        OpenMP::OpenMP_CXX
        ${VTK_LIBRARIES}
        )