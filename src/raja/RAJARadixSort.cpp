#include "RAJARadixSort.hpp"

#include <cmath>

RAJARadixSort::RAJARadixSort(std::vector<int> values,
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

auto RAJARadixSort::get_sorted_values() -> std::vector<int> {
  return sorted_values;
}

auto RAJARadixSort::get_key(int index) -> int {
  return (values[index] / this->pow10[iteration]) % 10;
}

void RAJARadixSort::calculate_counts(int block_number, int start_index,
                                     int end_index) {
  for (int i = start_index; i < end_index; i++) {
    int key = get_key(i);
    counts[key + block_number * NUM_BUCKETS]++;
  }
}

void RAJARadixSort::calculate_offsets() {
  RAJA::exclusive_scan<policy>(
      RAJA::make_span(&transpose_counts[0], num_values * NUM_BUCKETS),
      RAJA::make_span(&transpose_offsets[0], num_values * NUM_BUCKETS),
      RAJA::operators::plus<int>{}, 0);
}

void RAJARadixSort::place_values(int block_number, int start_index,
                                 int end_index) {
  for (int i = start_index; i < end_index; i++) {
    int key = get_key(i);
    sorted_values[offsets[key + block_number * NUM_BUCKETS]++] = values[i];
  }
}

void RAJARadixSort::transpose(const std::vector<int> &input,
                              std::vector<int> &output, int M, int N) {
  const int *in = &input[0];
  int *out = &output[0];

  RAJA::forall<policy>(RAJA::TypedRangeSegment<int>(0, M * N), [=](int i) {
    int row = i / M;
    int col = i % M;

    out[i] = input[col * N + row];
  });
}

void RAJARadixSort::sort() {
  for (; iteration < num_digits; iteration++) {
    RAJA::forall<policy>(
        RAJA::TypedRangeSegment<int>(0, num_blocks), [=](int i) {
          int block_start = i * block_length;
          int block_end = std::min((i + 1) * block_length, num_values);
          calculate_counts(i, block_start, block_end);
        });

    transpose(counts, transpose_counts, num_blocks, NUM_BUCKETS);
    calculate_offsets();
    transpose(transpose_offsets, offsets, NUM_BUCKETS, num_blocks);

    RAJA::forall<policy>(
        RAJA::TypedRangeSegment<int>(0, num_blocks), [=](int i) {
          int block_start = i * block_length;
          int block_end = std::min((i + 1) * block_length, num_values);
          place_values(i, block_start, block_end);
        });

    if (iteration != num_digits - 1) {
      RAJA::forall<policy>(RAJA::TypedRangeSegment<int>(0, offsets.size()),
                           [=](int i) {
                             offsets[i] = 0;
                             transpose_offsets[i] = 0;
                             counts[i] = 0;
                             transpose_counts[i] = 0;
                           });
      std::swap(values, sorted_values);
    }
  }
}
