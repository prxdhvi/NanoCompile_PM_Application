#include <algorithm>
#include <cassert>
#include <cstdint>
#include <optional>
#include <vector>
#include <chrono>
#include <iostream>

std::optional<std::vector<int64_t>> broadcastShape(
    const std::vector<int64_t>& a, const std::vector<int64_t>& b) {
  for (auto dim : a) {
    if (dim <= 0) return std::nullopt;
  }
  for (auto dim : b) {
    if (dim <= 0) return std::nullopt;
  }

  auto rankA = a.size();
  auto rankB = b.size();
  auto rankC = std::max(rankA, rankB);

  std::vector<int64_t> c(rankC);
  for (size_t i = 0; i < rankC; i++) {
    int64_t dim_a = 1;
    int64_t dim_b = 1;
    if (i < rankA) {
      dim_a = a[rankA - i - 1];
    }
    if (i < rankB) {
      dim_b = b[rankB - i - 1];
    }
    if (dim_a == dim_b || dim_a == 1 || dim_b == 1) {
      c[rankC - i - 1] = std::max(dim_a, dim_b);
    } else {
      return std::nullopt;
    }
  }
  return c;
}
void test(const std::vector<int64_t>& a, const std::vector<int64_t>& b,
          const std::optional<std::vector<int64_t>>& c) {
  assert(broadcastShape(a, b) == c);
}

void testSuite() {
  // Equal ranks
  test({1, 3, 1}, {1, 3, 1}, std::vector<int64_t>{1, 3, 1});
  test({1, 3, 4}, {2, 1, 4}, std::vector<int64_t>{2, 3, 4});

  test({2, 3, 4}, {2, 3, 4}, std::vector<int64_t>{2, 3, 4});

  test({2, 3, 4}, {2, 5, 4}, std::nullopt);
  // Different ranks

  test({6, 8}, {8}, std::vector<int64_t>{6, 8});

  test({4, 1, 8}, {8}, std::vector<int64_t>{4, 1, 8});

  test({5, 1, 7, 8}, {7, 8}, std::vector<int64_t>{5, 1, 7, 8});

  test({3, 4}, {2, 3, 4}, std::vector<int64_t>{2, 3, 4});

  // Scalars

  test({}, {}, std::vector<int64_t>{});

  test({}, {3, 4, 5}, std::vector<int64_t>{3, 4, 5});

  test({2, 7}, {}, std::vector<int64_t>{2, 7});

  // Mismatches

  test({2, 3}, {3, 2}, std::nullopt);

  test({5, 4, 3}, {5, 2, 3}, std::nullopt);

  test({8}, {7}, std::nullopt);

  // Invalid dimensions

  test({0, 3}, {1, 3}, std::nullopt);

  test({-1, 3}, {1, 3}, std::nullopt);

  test({2, 3}, {0, 3}, std::nullopt);

  test({1, 1, 1, 1, 1, 1, 1, 1}, {2, 3, 4, 5, 6, 7, 8, 9},
       std::vector<int64_t>{2, 3, 4, 5, 6, 7, 8, 9});

  test({2, 1, 4, 1, 8, 1, 16, 1}, {1, 3, 1, 5, 1, 7, 1, 9},
       std::vector<int64_t>{2, 3, 4, 5, 8, 7, 16, 9});

  // Wrong-side padding edge cases

  // Right-aligned broadcasting succeeds
  test({8}, {6, 8}, std::vector<int64_t>{6, 8});

  // Left-aligned interpretation would succeed
  test({8, 6}, {8}, std::nullopt);
}

/*
  The time complexity is O(rankA + rankB) since we are just linearly iterating over each of them

  we can benchmark by running for different ranks over a set of iterations to get avg time

*/

void benchmark() {
  const int N = 1e6;

  std::vector<int> ranks = {
    1,2,4,8,16,32,64,128,256,512
  };
  std::cout << " Ranks   Avg Time (ns)" << '\n';

  for(auto x : ranks) {
    std::vector<int64_t> a(x, 32);
    std::vector<int64_t> b(2,1);

    auto start = std::chrono::high_resolution_clock::now();
    for(int i =0;i<N;i++) {
      auto c = broadcastShape(a,b);
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto timeTaken  = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    auto avgTime = static_cast<double>(timeTaken) / N;

    std:: cout << x << "   " << std::fixed << std::setprecision(2) << avgTime << "\n";
  }

}

int main() {
  benchmark();
  return 0;
}
