#include "OMPRadixSort.hpp"
#include <cmath>
#include <numeric>

OMPRadixSort::OMPRadixSort(size_t num_digits, size_t num_values,
                           std::vector<int> input)
    : RadixSort(num_digits, num_values), values(input),
      sorted_values(num_values, 0) {
  block_size = num_values / M;
  if (block_size == 0) {
    block_size = 1;
  }
  num_blocks = std::ceil((float)num_values / (float)block_size);
  num_buckets = num_blocks * M;

  counts = std::vector<int>(num_buckets, 0);
  transpose_counts = std::vector<int>(num_buckets, 0);
  offsets = std::vector<int>(num_buckets, 0);
  transpose_offsets = std::vector<int>(num_buckets, 0);
}

auto OMPRadixSort::get_sorted_values() -> std::vector<int> {
  return sorted_values;
}

auto OMPRadixSort::get_key(size_t index) -> size_t {
  return (values[index] / pow10[iteration]) % 10;
}

void OMPRadixSort::calculate_counts() {
#pragma omp parallel for
  for (size_t block_number = 0; block_number < num_blocks; block_number++) {
    size_t start = block_number * block_size;
    size_t end = std::min((block_number + 1) * block_size, num_values);

    for (size_t i = start; i < end; i++) {
      size_t key = get_key(i);
      counts[key + block_number * M]++;
    }
  }
}

void OMPRadixSort::calculate_offsets() {
  transpose(counts, transpose_counts, num_blocks, M);
  std::exclusive_scan(transpose_counts.cbegin(), transpose_counts.cend(),
                      transpose_offsets.begin(), 0);
  transpose(transpose_offsets, offsets, M, num_blocks);
}

void OMPRadixSort::place_values() {
#pragma omp parallel for
  for (size_t block_number = 0; block_number < num_blocks; block_number++) {
    size_t start = block_number * block_size;
    size_t end = std::min((block_number + 1) * block_size, num_values);

    for (size_t i = start; i < end; i++) {
      size_t key = get_key(i);
      sorted_values[offsets[key + block_number * M]++] = values[i];
    }
  }
}

void OMPRadixSort::reset() {
#pragma omp parallel for
  for (size_t i = 0; i < num_buckets; i++) {
    counts[i] = 0;
  }

  std::swap(values, sorted_values);
}

void OMPRadixSort::transpose(const std::vector<int> &input,
                             std::vector<int> &output, size_t num_rows,
                             size_t num_cols) {
  for (size_t i = 0; i < num_buckets; i++) {
    size_t row = i / num_rows;
    size_t col = i % num_rows;
    output[i] = input[col * num_cols + row];
  }
}
