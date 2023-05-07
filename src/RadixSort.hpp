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

  std::vector<int> pow10 = {1,      10,      100,      1000,      10000,
                            100000, 1000000, 10000000, 100000000, 1000000000};
};
