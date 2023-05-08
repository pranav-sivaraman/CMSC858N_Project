#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

#include "OMPRadixSort.hpp"
#include "RAJARadixSort.hpp"
#include "RadixSort.hpp"
#include "SerialRadixSort.hpp"

void print_vector(const std::vector<int> v) {
  for (auto element : v) {
    std::cout << element << ", ";
  }
  std::cout << "\n";
}

auto main() -> int {
  int max_digits = 4;
  int N = 10;

  std::vector<int> data(N);
  std::vector<int> sorted_data(N);

  std::random_device rnd_device;
  std::mt19937 mersenne_engine{rnd_device()}; // Generates random integers
  std::uniform_int_distribution<int> dist{0, 1000};

  auto gen = [&dist, &mersenne_engine]() { return dist(mersenne_engine); };

  std::generate(data.begin(), data.end(), gen);

  const std::string model_names[3] = {"Serial", "OMP", "RAJA"};

  std::unique_ptr<RadixSort> model =
      std::make_unique<RAJARadixSort>(data, sorted_data, max_digits, N);

  int num_models = 1;
  std::cout << "Original Data: ";
  print_vector(data);
  for (int i = 0; i < num_models; i++) {
    model->sort();
    sorted_data = model->get_sorted_values();
    bool is_sorted = std::is_sorted(sorted_data.cbegin(), sorted_data.cend());

    if (is_sorted) {
      std::cout << "Data is sorted!\n";
    } else {
      std::cout << "Data is not sorted!\n";
    }

    std::cout << "Sorted Data: ";
    print_vector(sorted_data);
  }

  return 0;
}
