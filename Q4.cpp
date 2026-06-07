#include <algorithm>
#include <iostream>
#include <vector>

void matmul_tiled(const float* A, const float* B, float* C, int M, int N, int K,int TILE) {
    // A - M x K, B - K x N, C - M x N
    // Cik += Aij * Bjk


    for(int i_ = 0; i_ < M; i_+= TILE) {
        int iend = std::min(i_+TILE,M);
        for(int j_ = 0;j_<K;j_+= TILE) {
            int jend = std::min(j_+TILE,K);
            for(int k_ =0;k_<N;k_+=TILE) {
                int kend = std::min(k_+TILE,N);
                for(int i = i_;i<iend;i++) {
                    for(int j = j_;j<jend;j++) {

                        float a = A[i*K + j];
                        for(int k = k_;k<kend;k++) {
                            C[i*N + k] += a*B[j*N + k];
                        }
                    }
                }
            }
        }
    }
}

int main() {
  const int M = 2;
  const int K = 3;
  const int N = 2;

  std::vector<float> A = {1, 2, 3, 4, 5, 6};

  std::vector<float> B = {7, 8, 9, 10, 11, 12};

  std::vector<float> C(M * N, 0);

  matmul_tiled(A.data(), B.data(), C.data(), M, N, K, 2);

  std::cout << "Result:\n";

  for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
      std::cout << C[i * N + j] << " ";
    }
    std::cout << '\n';
  }

  return 0;
}