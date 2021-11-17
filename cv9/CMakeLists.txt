cmake_minimum_required(VERSION 3.20)
project(cv9)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)
find_package(VTK REQUIRED)

include(${VTK_USE_FILE})

# CXXOPTS
include(FetchContent)
FetchContent_Declare(
        cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
        GIT_TAG        v2.2.1
        GIT_SHALLOW    TRUE
)
set(CXXOPTS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CXXOPTS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CXXOPTS_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(CXXOPTS_ENABLE_WARNINGS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(cxxopts)


add_executable(cv9 main.cpp
        Grid.cpp Grid.h
        Particle.h
        GridPoint.cpp GridPoint.h
        GridCell.cpp GridCell.h
        Options.h Options.cpp
        )

target_link_libraries(cv9
        cxxopts
        #OpenMP::OpenMP_CXX
        ${VTK_LIBRARIES}
        )

add_executable(test test.cpp)

target_link_libraries(test
        cxxopts
        OpenMP::OpenMP_CXX
        ${VTK_LIBRARIES}
        )

add_executable(process_all process_all.cpp)

target_link_libraries(process_all OpenMP::OpenMP_CXX)