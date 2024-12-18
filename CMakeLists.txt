cmake_minimum_required(VERSION 3.29)
project(MatrixMultiplication)

set(CMAKE_CXX_STANDARD 20)

add_executable(MatrixMultiplication main.cpp)
target_compile_options(MatrixMultiplication PRIVATE -O3 -march=native)