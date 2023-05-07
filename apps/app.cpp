#include <algorithm>
#include <iostream>
#include <vector>

#include "RadixSort.hpp"
#include "SerialRadixSort.hpp"

void print_vector(const std::vector<int> v) {
  for (auto element : v) {
    std::cout << element << ", ";
  }
  std::cout << "\n";
}

auto main() -> int {
  int max_digits = 2;
  int N = 10;
  std::vector<int> data = {0, 5, 2, 3, 4, 10, 4, 2, 1, 4};
  std::vector<int> sorted_data(N);

  std::unique_ptr<RadixSort> model =
      std::make_unique<SerialRadixSort>(data, sorted_data, max_digits, N);

  int num_models = 1;
  for (int i = 0; i < num_models; i++) {
    model->sort();
    sorted_data = model->get_sorted_values();
    bool is_sorted = std::is_sorted(sorted_data.cbegin(), sorted_data.cend());

    if (is_sorted) {
      std::cout << "Data is sorted!\n";
    } else {
      std::cout << "Data is not sorted!\n";
    }
  }

  return 0;
}
