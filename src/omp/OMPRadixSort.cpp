#include "OMPRadixSort.hpp"

#include <cmath>

OMPRadixSort::OMPRadixSort(std::vector<int> values,
                           std::vector<int> sorted_values, int num_digits,
                           int num_values)
    : values(std::move(values)), sorted_values(std::move(sorted_values)) {
  this->num_digits = num_digits;
  this->num_values = num_values;
  this->block_length = num_digits / NUM_BUCKETS;
  if (this->block_length == 0) {
    block_length = 1;
  }
  this->num_blocks = std::ceil(num_values / block_length);

  offsets = std::vector<int>(num_blocks * NUM_BUCKETS);
  counts = std::vector<int>(num_blocks * NUM_BUCKETS);
  transpose_offsets = std::vector<int>(num_blocks * NUM_BUCKETS);
  transpose_counts = std::vector<int>(num_blocks * NUM_BUCKETS);
}

auto OMPRadixSort::get_sorted_values() -> std::vector<int> {
  return sorted_values;
}

auto OMPRadixSort::get_key(int index) -> int {
  return (values[index] / this->pow10[iteration]) % 10;
}

void OMPRadixSort::calculate_counts(int block_number, int start_index,
                                    int end_index) {
  for (int i = start_index; i < end_index; i++) {
    int key = get_key(i);
    counts[key + block_number * NUM_BUCKETS]++;
  }
}

void OMPRadixSort::calculate_offsets() {
  int x = 0;
#pragma omp parallel for reduction(inscan, + : x)
  for (int i = 0; i < NUM_BUCKETS * num_blocks; i++) {
    transpose_offsets[i] = x;
#pragma omp scan exclusive(x)
    x += transpose_counts[i];
  }
}

void OMPRadixSort::place_values(int block_number, int start_index,
                                int end_index) {
  for (int i = start_index; i < end_index; i++) {
    int key = get_key(i);
    sorted_values[offsets[key + block_number * NUM_BUCKETS]++] = values[i];
  }
}

void OMPRadixSort::transpose(const std::vector<int> &input,
                             std::vector<int> &output, int M, int N) {
#pragma omp parallel for
  for (int i = 0; i < M * N; i++) {
    int row = i / M;
    int col = i % M;

    output[i] = input[col * N + row];
  }
}

void OMPRadixSort::sort() {
  for (; iteration < num_digits; iteration++) {
#pragma omp parallel for
    for (int i = 0; i < num_blocks; i++) {
      int block_start = i * block_length;
      int block_end = std::min((i + 1) * block_length, num_values);
      calculate_counts(i, block_start, block_end);
    }

    transpose(counts, transpose_counts, num_blocks, NUM_BUCKETS);
    calculate_offsets();
    transpose(transpose_offsets, offsets, NUM_BUCKETS, num_blocks);

#pragma omp parallel for
    for (int i = 0; i < num_blocks; i++) {
      int block_start = i * block_length;
      int block_end = std::min((i + 1) * block_length, num_values);
      place_values(i, block_start, block_end);
    }

    if (iteration != num_digits - 1) {
#pragma omp parallel for
      for (int i = 0; i < offsets.size(); i++) {
        offsets[i] = 0;
        transpose_offsets[i] = 0;
        counts[i] = 0;
        transpose_counts[i] = 0;
      }
      std::swap(values, sorted_values);
    }
  }
}
