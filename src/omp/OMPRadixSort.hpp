#pragma once

#include "RadixSort.hpp"

class OMPRadixSort : public RadixSort {
public:
  OMPRadixSort(size_t num_digits, size_t num_values, std::vector<int> input);
  virtual auto get_sorted_values() -> std::vector<int> override;

protected:
  size_t num_blocks;
  size_t block_size;
  size_t num_buckets;

  std::vector<int> values;
  std::vector<int> sorted_values;
  std::vector<int> counts;
  std::vector<int> transpose_counts;
  std::vector<int> offsets;
  std::vector<int> transpose_offsets;

  auto get_key(size_t index) -> size_t override;
  void calculate_counts() override;
  void calculate_offsets() override;
  void place_values() override;
  void reset() override;

  void transpose(const std::vector<int> &input, std::vector<int> &output,
                 size_t num_rows, size_t num_cols);
};
