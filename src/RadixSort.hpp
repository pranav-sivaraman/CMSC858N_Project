#pragma once

#include <vector>

#define NUM_BUCKETS 10

class RadixSort {
public:
  virtual ~RadixSort() = default;
  virtual void sort() = 0;
  virtual auto get_sorted_values() -> std::vector<int> = 0;

protected:
  int num_digits{};
  int num_values{};
  int iteration = 0;

  virtual void calculate_counts() = 0;
  virtual void calculate_offsets() = 0;
  virtual void place_values() = 0;
  virtual auto get_key(int index) -> int = 0;
};
