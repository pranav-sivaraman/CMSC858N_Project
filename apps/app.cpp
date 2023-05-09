#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

#include "KokkosRadixSort.hpp"
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

auto main(int argc, char *argv[]) -> int {
  int max_digits = 5;
  int N = 1000;

  if (argc >= 2) {
    N = std::stoi(argv[1]);
  }

  std::vector<int> data(N);
  std::vector<int> sorted_data(N);

  std::random_device rnd_device;
  std::mt19937 mersenne_engine{rnd_device()}; // Generates random integers
  std::uniform_int_distribution<int> dist{0, 10000};

  auto gen = [&dist, &mersenne_engine]() { return dist(mersenne_engine); };

  std::generate(data.begin(), data.end(), gen);

  const std::string model_names[4] = {"Serial", "OMP", "RAJA", "Kokkos"};

  std::unique_ptr<RadixSort> model =
      std::make_unique<SerialRadixSort>(data, sorted_data, max_digits, N);

  std::chrono::steady_clock::time_point start;
  std::chrono::steady_clock::time_point end;
  std::chrono::duration<double> elapsed_seconds;

  int num_models = 1;
  for (int i = 0; i < num_models; i++) {
    start = std::chrono::steady_clock::now();
    model->sort();
    end = std::chrono::steady_clock::now();

    elapsed_seconds = end - start;

    sorted_data = model->get_sorted_values();
    bool is_sorted = std::is_sorted(sorted_data.cbegin(), sorted_data.cend());

    if (is_sorted) {
      std::cout << "Data is sorted!\n";
      std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    } else {
      std::cout << "Data is not sorted!\n";
    }
  }

  return 0;
}
