#include "RAJARadixSort.hpp"
#include <cmath>
#include <numeric>

RAJARadixSort::RAJARadixSort(size_t num_digits, size_t num_values,
                             std::vector<int> input)
    : RadixSort(num_digits, num_values), values(input),
      sorted_values(num_values, 0), counts(num_buckets, 0),
      transpose_counts(num_buckets, 0), offsets(num_buckets, 0),
      transpose_offsets(num_buckets, 0) {}

auto RAJARadixSort::get_sorted_values() -> std::vector<int> {
  return sorted_values;
}

auto RAJARadixSort::get_key(size_t index) -> size_t {
  return (values[index] / pow10[iteration]) % 10;
}

void RAJARadixSort::calculate_counts() {
  RAJA::forall<policy>(
      RAJA::TypedRangeSegment<size_t>(0, num_blocks), [&](size_t block_number) {
        size_t start = block_number * block_size;
        size_t end = std::min((block_number + 1) * block_size, num_values);

        for (size_t i = start; i < end; i++) {
          size_t key = get_key(i);
          counts[key + block_number * NUM_KEYS]++;
        }
      });
}

void RAJARadixSort::calculate_offsets() {
  transpose(counts, transpose_counts, num_blocks, NUM_KEYS);
  scan();
  transpose(transpose_offsets, offsets, NUM_KEYS, num_blocks);
}

void RAJARadixSort::place_values() {
  // #pragma omp parallel for
  for (size_t block_number = 0; block_number < num_blocks; block_number++) {
    size_t start = block_number * block_size;
    size_t end = std::min((block_number + 1) * block_size, num_values);

    for (size_t i = start; i < end; i++) {
      size_t key = get_key(i);
      sorted_values[offsets[key + block_number * NUM_KEYS]++] = values[i];
    }
  }
}

void RAJARadixSort::reset() {
  // #pragma omp parallel for
  for (size_t i = 0; i < num_buckets; i++) {
    counts[i] = 0;
  }

  std::swap(values, sorted_values);
}

void RAJARadixSort::scan() {
  int sum = 0;
  // #pragma omp parallel for reduction(inscan, + : sum)
  for (int k = 0; k < num_buckets; k++) {
    transpose_offsets[k] = sum;
    // #pragma omp scan exclusive(sum)
    sum += transpose_counts[k];
  }
}

void RAJARadixSort::transpose(const std::vector<int> &input,
                              std::vector<int> &output, size_t num_rows,
                              size_t num_cols) {
  // #pragma omp parallel for
  for (size_t i = 0; i < num_buckets; i++) {
    size_t row = i / num_rows;
    size_t col = i % num_rows;
    output[i] = input[col * num_cols + row];
  }
}
