#pragma once

#include <RadixSort.hpp>

class SerialRadixSort : public RadixSort {
public:
  SerialRadixSort(size_t num_digits, size_t num_values,
                  std::vector<int> values);

  auto get_sorted_values() -> std::vector<int> override;

protected:
  std::vector<int> values;
  std::vector<int> sorted_values;
  std::vector<int> counts;
  std::vector<int> offsets;

  auto get_key(size_t index) -> size_t override;
  void calculate_counts() override;
  void calculate_offsets() override;
  void place_values() override;
  void reset() override;
  void scan() override;
};
