#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <chrono>
#include <latch>
#include <cstring>

#define Matrix std::vector<double>

// Function to generate a random matrix of size N x N
Matrix generate_matrix(int N) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(1, 10);

    Matrix matrix(N*N, 0.0);
    for(int i = 0; i < N*N; ++i) {
        matrix[i] = dist(gen);
    }

    return matrix;
}

inline int index(int r, int c, int N) {
    return r*N + c;
}

// execute summation in tiles to improve spatial locality in cache
void worker(const Matrix& A, const Matrix& B, Matrix& C, int start_row, int end_row,
                   int N, int tile_size, std::latch& worker_latch) {
    // iterate through matrices of tile_size*tile_size to increase cache locality
    for (int i = start_row; i < end_row; i += tile_size) {
        for (int j = 0; j < N; j += tile_size) {
            for (int k = 0; k < N; k += tile_size) {
                // find product of tiles A and B^T
                int _b1 = std::min(i + tile_size, end_row),
                    _b2 = std::min(j + tile_size, N),
                    _b3 = std::min(k + tile_size, N);
                for (int ii = i; ii < _b1; ++ii) {
                    for (int jj = j; jj < _b2; ++jj) {
                        double sum = C[index(ii, jj, N)];
                        for (int kk = k; kk < _b3; ++kk) {
                            sum += A[index(ii,kk, N)] * B[index(jj,kk,N)];
                        }
                        C[index(ii, jj, N)] = sum;
                    }
                }

            }
        }
    }
    worker_latch.count_down();
}

void transpose(Matrix& m, int N) {
    for(int i = 0; i < N; ++i) {
        for(int j = i; j < N; ++j) {
            std::swap(m[i*N + j], m[j*N + i]);
        }
    }
}

inline void printMatrix(const Matrix& m, int N) {
    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < N; ++j) {
            std::cout << m[i*N + j] << " ";
        }
        std::cout << "\n";
    }
}

// simple matrix mult (A*B^T) single threaded to check correctness
Matrix classicMatrixMult(const Matrix& A, const Matrix& B, int N) {
    Matrix C(N*N, 0.0);

    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < N; ++j) {
            for(int k = 0; k < N; ++k) {
                C[i*N + j] += A[i*N + k] * B[j*N + k];
            }
        }
    }

    return C;
}

int main(int argc, char* argv[]) {
    int N, thread_count, tile_size;
    bool print_matrices = false;
    try {
        N = std::stoi(argv[1]);
        thread_count = std::stoi(argv[2]);
        tile_size = std::stoi(argv[3]);

        print_matrices = (std::strcmp(argv[4], "true") == 0);
        if (N <= 0 || thread_count <= 0 || tile_size <= 0) {
            throw std::runtime_error("Matrix size, thread count, and tile size must be positive integers");
        }
    } catch(const std::exception& err) {
        std::cerr << "Usage: " << argv[0] << " <matrix_size> <thread_count> <tile_size> <print_output>\n" << err.what() << "\nDefaulting to N=1024 on 16 Threads, tile_size=32\n";
        N = 1024;
        thread_count = 16;
        tile_size = 32;
    }

    std::latch await_workers_latch(thread_count);

    // Generate two random matrices
    Matrix A = generate_matrix(N);
    Matrix B = generate_matrix(N);
    transpose(B, N); // transpose B to increase spatial locality in tiled multiplication
    Matrix C(N*N, 0.0);

    auto start_time = std::chrono::high_resolution_clock::now();

    // Create threads and assign work
    std::vector<std::jthread> threads;
    threads.reserve(thread_count);
    int rows_per_thread = N / thread_count;
    int extra_rows = N % thread_count; // Handle remainder rows

    int start_row = 0;
    for (int t = 0; t < thread_count; ++t) {
        int end_row = start_row + rows_per_thread + (t < extra_rows ? 1 : 0);
        threads.emplace_back([&, start_row, end_row]() {
            worker(A, B, C, start_row, end_row, N, tile_size, await_workers_latch);
        });
        start_row = end_row;
    }
    await_workers_latch.wait();

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    if (print_matrices) {
        Matrix d = classicMatrixMult(A, B, N);

        std::cout << "A:\n";
        printMatrix(A, N);
        std::cout << "\n\nB:\n";
        transpose(B, N); // transpose back for printing
        printMatrix(B, N);
        std::cout << "\n\nC = A*B\n";
        printMatrix(C, N);
        std::cout << "\n\nD = A*B (single threaded)\n";
        printMatrix(d, N);
    }
    std::cout << std::endl << N << "x" << N << " Matrix multiplication completed in " << elapsed.count() << " seconds.\n";

    return 0;
}