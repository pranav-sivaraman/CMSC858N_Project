#pragma once

#include <vector>

#include "../RadixSort.hpp"
#include <CL/sycl.hpp>

class SYCLRadixSort : public RadixSort {
public:
  SYCLRadixSort(std::vector<int> values, std::vector<int> sorted_values,
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

  cl::sycl::queue q{};

  void calculate_counts();
  void place_values();
  void calculate_offsets();
  void transpose(const std::vector<int> &input, std::vector<int> &output, int M,
                 int N);
  auto get_key(int index) -> int;

  int num_blocks;
  int block_length;
};
