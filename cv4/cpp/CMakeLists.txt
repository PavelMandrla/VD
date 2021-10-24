cmake_minimum_required(VERSION 3.20)
project(cv4)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED)

add_executable(cv4 main.cpp)

target_link_libraries(cv4
        ${OpenCV_LIBS}
        OpenMP::OpenMP_CXX
        )