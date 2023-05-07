#include <iostream>
#include <numeric>

#include "SerialRadixSort.hpp"

SerialRadixSort::SerialRadixSort(std::vector<int> values,
                                 std::vector<int> sorted_values, int num_digits,
                                 int num_values)
    : values(std::move(values)), sorted_values(std::move(sorted_values)),
      offsets(NUM_BUCKETS, 0), counts(NUM_BUCKETS, 0) {
  this->num_digits = num_digits;
  this->num_values = num_values;
}

auto SerialRadixSort::get_key(int index) -> int {
  return (values[index] / this->pow10[iteration]) % 10;
}

void SerialRadixSort::calculate_counts() {
  for (int i = 0; i < this->num_values; i++) {
    int key = get_key(i);
    counts[key]++;
  }
}

void SerialRadixSort::place_values() {
  for (int i = 0; i < num_values; i++) {
    int key = get_key(i);
    sorted_values[offsets[key]++] = values[i];
  }
}

void SerialRadixSort::calculate_offsets() {
  std::exclusive_scan(counts.cbegin(), counts.cend(), offsets.begin(), 0);
}

auto SerialRadixSort::get_sorted_values() -> std::vector<int> {
  return sorted_values;
}

void SerialRadixSort::sort() {
  for (; iteration < num_digits; iteration++) {
    calculate_counts();
    calculate_offsets();
    place_values();

    std::fill(counts.begin(), counts.end(), 0);
    std::fill(offsets.begin(), offsets.end(), 0);

    if (iteration != num_digits - 1) {
      std::vector<int> tmp = values;
      values = sorted_values;
      sorted_values = tmp;
    }
  }
}
