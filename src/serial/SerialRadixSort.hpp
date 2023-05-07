#pragma once

#include <vector>

#include "../RadixSort.hpp"

class SerialRadixSort : public RadixSort {
public:
  SerialRadixSort(std::vector<int> values, std::vector<int> sorted_values,
                  int num_digits, int num_values);
  void sort() override;
  auto get_sorted_values() -> std::vector<int> override;

private:
  std::vector<int> values;
  std::vector<int> sorted_values;
  std::vector<int> offsets;
  std::vector<int> counts;

  std::vector<int> pow10 = {1,      10,      100,      1000,      10000,
                            100000, 1000000, 10000000, 100000000, 1000000000};

  void calculate_counts() override;
  void calculate_offsets() override;
  void place_values() override;
  auto get_key(int index) -> int override;
};
