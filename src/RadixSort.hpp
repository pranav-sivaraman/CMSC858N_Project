#pragma once

#include <array>
#include <iostream>
#include <vector>

#define M 10

class RadixSort {
public:
  RadixSort(size_t num_digits, size_t num_values)
      : num_digits(num_digits), num_values(num_values){};
  virtual ~RadixSort() = default;

  virtual void sort() {
    for (; iteration < num_digits; iteration++) {
      calculate_counts();
      calculate_offsets();
      place_values();

      if (iteration != num_digits - 1) {
        reset();
      }
    }
  }

  virtual auto get_sorted_values() -> std::vector<int> = 0;

  void print_vector(const std::vector<int> v) {
    for (auto i : v) {
      std::cout << i << " ";
    }
    std::cout << "\n";
  }

protected:
  size_t num_digits;
  size_t num_values;
  size_t iteration = 0;

  std::array<int, M> pow10 = {1,      10,      100,      1000,      10000,
                              100000, 1000000, 10000000, 100000000, 1000000000};

  virtual auto get_key(size_t index) -> size_t = 0;
  virtual void calculate_counts() = 0;
  virtual void calculate_offsets() = 0;
  virtual void place_values() = 0;
  virtual void reset() = 0;
};
