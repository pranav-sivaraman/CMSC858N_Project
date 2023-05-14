#include <numeric>

#include "SerialRadixSort.hpp"

SerialRadixSort::SerialRadixSort(size_t num_digits, size_t num_values,
                                 std::vector<int> input)
    : RadixSort(num_digits, num_values), values(input),
      sorted_values(num_values, 0), counts(NUM_KEYS, 0), offsets(NUM_KEYS, 0){};

auto SerialRadixSort::get_sorted_values() -> std::vector<int> {
  return sorted_values;
}

auto SerialRadixSort::get_key(size_t index) -> size_t {
  return (values[index] / pow10[iteration]) % 10;
}

void SerialRadixSort::calculate_counts() {
  for (size_t i = 0; i < num_values; i++) {
    size_t key = get_key(i);
    counts[key]++;
  }
}

void SerialRadixSort::calculate_offsets() { scan(); }

void SerialRadixSort::place_values() {
  for (size_t i = 0; i < num_values; i++) {
    size_t key = get_key(i);
    sorted_values[offsets[key]++] = values[i];
  }
}

void SerialRadixSort::scan() {
  std::exclusive_scan(counts.cbegin(), counts.cend(), offsets.begin(), 0);
}

void SerialRadixSort::reset() {
  std::fill(counts.begin(), counts.end(), 0);
  std::swap(values, sorted_values);
}
