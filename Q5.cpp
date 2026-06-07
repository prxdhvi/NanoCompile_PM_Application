#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <numeric>

void sum(const std::vector<int>& a, size_t start, size_t end, long long& sum) {
    long long ans = 0;

    for(auto i = start;i<end;i++) {
        ans += a[i];
    }
    sum = ans;
}

int main() {
    size_t N = 1e8;
    unsigned int  n_threads = std::max(1u,std::thread::hardware_concurrency());

    std::vector<int> arr(N);

    std::mt19937 rnd(67);
    std::uniform_int_distribution<int> dist(1,100);
    for (auto i = 0uz; i < N; i++) {
      arr[i] = dist(rnd);
    }

    // Single Threaded version :
    {
      long long sum = 0;
      auto start = std::chrono::high_resolution_clock::now();

      for (int x : arr) {
        sum += x;
      }
        auto end = std::chrono::high_resolution_clock::now();

        auto timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "Single Threaded Sum: " << sum << "\n";
        std::cout << "Single Threaded Time Taken: " << timeTaken.count()  << "ms" << "\n";
    }
    // Multi-Threaded version :
    {
        std::cout << "No.of Threads: " << n_threads << "\n";

        std::vector<std::thread> threads;
        std::vector<long long> sums(n_threads,0);

        size_t block_size = N / n_threads;

        auto start = std::chrono::high_resolution_clock::now();

        for(auto i = 0u;i<n_threads;i++) {
            auto start_idx = i*block_size;

            auto end_idx = i==n_threads-1 ? N : start_idx + block_size;

            threads.emplace_back(sum,std::cref(arr),start_idx,end_idx,std::ref(sums[i]));
        }
        for(auto& thread : threads) {
            thread.join();
        }
        long long sum = 0;
        for(auto x : sums) {
            sum +=x;
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "Multi-Threaded Sum: " << sum << "\n";
        std::cout << "Multi-Threaded Time Taken: " << timeTaken.count() << "ms" << "\n";
    }
    /*
    Single Threaded Sum: 5050529259
    Single Threaded Time Taken: 165ms
    No.of Threads: 20
    Multi-Threaded Sum: 5050529259
    Multi-Threaded Time Taken: 50ms
    */
    return 0;
}
