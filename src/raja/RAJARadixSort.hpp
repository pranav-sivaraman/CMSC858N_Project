#pragma once

#include "../RadixSort.hpp"
#include <RAJA/RAJA.hpp>
#include <vector>

class RAJARadixSort : public RadixSort {
public:
  RAJARadixSort(std::vector<int> values, std::vector<int> sorted_values,
                int num_digits, int num_values);
  void sort() override;
  auto get_sorted_values() -> std::vector<int> override;

private:
  std::vector<int> values;
  std::vector<int> sorted_values;
  std::vector<int> offsets;
  std::vector<int> counts;
  std::vector<int> transpose_counts;
  std::vector<int> transpose_offsets;

  using policy = RAJA::omp_parallel_for_exec;

  void calculate_counts(int block_number, int start_index, int end_index);
  void place_values(int block_number, int start_index, int end_index);
  void calculate_offsets();
  void transpose(const std::vector<int> &input, std::vector<int> &output, int M,
                 int N);
  auto get_key(int index) -> int;

  int num_blocks;
  int block_length;
};
