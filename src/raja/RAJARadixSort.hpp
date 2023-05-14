#pragma once

#include "RadixSort.hpp"
#include <RAJA/RAJA.hpp>

class RAJARadixSort : public RadixSort {
public:
  RAJARadixSort(size_t num_digits, size_t num_values, std::vector<int> input);
  virtual auto get_sorted_values() -> std::vector<int> override;

protected:
  std::vector<int> values;
  std::vector<int> sorted_values;
  std::vector<int> counts;
  std::vector<int> transpose_counts;
  std::vector<int> offsets;
  std::vector<int> transpose_offsets;

  using policy = RAJA::omp_parallel_for_exec;

  auto get_key(size_t index) -> size_t override;
  void calculate_counts() override;
  void calculate_offsets() override;
  void place_values() override;
  void reset() override;
  void scan() override;

  void transpose(const std::vector<int> &input, std::vector<int> &output,
                 size_t num_rows, size_t num_cols);
};
