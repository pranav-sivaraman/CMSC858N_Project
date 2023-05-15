#pragma once

#include <sycl/sycl.hpp>

#include <RadixSort.hpp>

class SYCLRadixSort : public RadixSort {
public:
  SYCLRadixSort(size_t num_digits, size_t num_values, std::vector<int> input);
  virtual auto get_sorted_values() -> std::vector<int> override;

protected:
  sycl::queue q{};
  sycl::buffer<int> values;

  sycl::buffer<int> sorted_values;
  sycl::buffer<int> partial_sums;
  sycl::buffer<int> counts;
  sycl::buffer<int> transpose_counts;
  sycl::buffer<int> offsets;
  sycl::buffer<int> transpose_offsets;
  sycl::buffer<int> bpow10;

  auto get_key(size_t index) -> size_t override;
  void calculate_counts() override;
  void calculate_offsets() override;
  void place_values() override;
  void reset() override;
  void scan() override;
};
