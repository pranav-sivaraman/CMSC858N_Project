#pragma once

#include "RadixSort.hpp"
#include <Kokkos_Core.hpp>

class KokkosRadixSort : public RadixSort {
public:
  KokkosRadixSort(size_t num_digits, size_t num_values, std::vector<int> input);
  ~KokkosRadixSort();
  virtual auto get_sorted_values() -> std::vector<int> override;

protected:
  Kokkos::View<int *> values;
  Kokkos::View<int *> sorted_values;
  Kokkos::View<int *> counts;
  Kokkos::View<int *> transpose_counts;
  Kokkos::View<int *> offsets;
  Kokkos::View<int *> transpose_offsets;

  auto get_key(size_t index) -> size_t override;
  void calculate_counts() override;
  void calculate_offsets() override;
  void place_values() override;
  void reset() override;
  void scan() override;

  void transpose(const Kokkos::View<int *> &input, Kokkos::View<int *> &output,
                 size_t num_rows, size_t num_cols);
};
