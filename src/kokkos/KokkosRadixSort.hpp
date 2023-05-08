#pragma once

#include "../RadixSort.hpp"
#include <Kokkos_Core.hpp>
#include <vector>

class KokkosRadixSort : public RadixSort {
public:
  KokkosRadixSort(std::vector<int> values, std::vector<int> sorted_values,
                  int num_digits, int num_values);
  ~KokkosRadixSort();
  void sort() override;
  auto get_sorted_values() -> std::vector<int> override;

private:
  Kokkos::View<int *> *values;
  Kokkos::View<int *> *sorted_values;
  Kokkos::View<int *> *offsets;
  Kokkos::View<int *> *counts;
  Kokkos::View<int *> *transpose_counts;
  Kokkos::View<int *> *transpose_offsets;

  void calculate_counts(int block_number, int start_index, int end_index);
  void place_values(int block_number, int start_index, int end_index);
  void calculate_offsets();
  void transpose(const Kokkos::View<int *> &input, Kokkos::View<int *> &output,
                 int M, int N);
  auto get_key(int index) -> int;

  int num_blocks;
  int block_length;
};
