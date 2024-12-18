# Multithreaded Matrix Multiplication

This is a simple multithreaded program for matrix multiplication, optimized for performance using configurable thread counts, tile sizes for cache locality, and optional output display.

## Functionality

- Takes 4 arguments:
    - `<matrix_size>` Determines the dimensions of the NxN matrices to multiply
    - `<thread_count>` Sets the number of threads to defer computation to
    - `<tile_size>` Sets the tile size for improving cache locality (defaults to 32)
    - `<print?true/false>` true/false for printing out the matrices and their product


- Creates 2 NxN matrices, randomizes their values and multiplies them
- Benchmarks the time it takes to compute the matrix multiplication

## Features

- Multithreaded matrix multiplication for improved performance.
- Configurable tile size to enhance cache locality.
- Adjustable thread count for multithreading flexibility.
- Option to print the resulting matrices.

## Running the Program

- Both a makefile and CMake are provided to build the project
- After building with the makefile, the program can be run with:

`./MatrixMult.exe <matrix_size> <thread_count> <tile_size> <print?true/false>`

## Testing

The validity of the program output has been checked against a naive approach known to be accurate. This debug method is active when `print` is set to True, where both the multithreaded algorithm and naive approach are executed sequentially.

#### This project is open source and free to use. It is available [here.](https://github.com/Lukas-Petervary/MatrixMultiplication)