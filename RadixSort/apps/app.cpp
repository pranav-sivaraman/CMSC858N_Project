#include <chrono>
#include <iostream>
#include <random>

#include "KokkosRadixSort.hpp"
#include "OMPRadixSort.hpp"
#include "RAJARadixSort.hpp"
#include "SYCLRadixSort.hpp"
#include "SerialRadixSort.hpp"

void print_vector(const std::vector<int> v) {
  for (auto i : v) {
    std::cout << i << " ";
  }
  std::cout << "\n";
}

auto main(int argc, char *argv[]) -> int {
  size_t max_digits = 5;
  size_t N = 10;

  if (argc >= 2) {
    N = std::stoi(argv[1]);
  }

  std::vector<int> input(N);

  std::random_device rnd_device;
  std::mt19937 mersenne_engine{rnd_device()}; // Generates random integers
  std::uniform_int_distribution<int> dist{0, 10000};

  auto gen = [&dist, &mersenne_engine]() { return dist(mersenne_engine); };

  std::generate(input.begin(), input.end(), gen);

  std::vector<const std::string> model_names = {"Serial", "OpenMP", "Kokkos",
                                                "RAJA", "SYCL"};
  RadixSort *models[] = {new SerialRadixSort(max_digits, N, input),
                         new OMPRadixSort(max_digits, N, input),
                         new KokkosRadixSort(max_digits, N, input),
                         new RAJARadixSort(max_digits, N, input),
                         new SYCLRadixSort(max_digits, N, input)};
  size_t num_models = model_names.size();

  std::chrono::steady_clock::time_point start;
  std::chrono::steady_clock::time_point end;
  std::chrono::duration<double> elapsed_seconds;

  std::vector<std::chrono::duration<double>> timings(num_models);

  for (size_t i = 0; i < num_models; i++) {
    start = std::chrono::steady_clock::now();
    models[i]->sort();
    end = std::chrono::steady_clock::now();
    elapsed_seconds = end - start;

    timings[i] = elapsed_seconds;

    std::vector<int> output = models[i]->get_sorted_values();

    if (std::is_sorted(output.cbegin(), output.cend())) {
      std::cout << "Data is sorted!\n";
    } else {
      std::cout << "Data is not sorted\n";
    }
  }

  std::cout << "Timings\n";
  for (int i = 0; i < num_models; i++) {
    std::cout << model_names[i] << " " << timings[i].count() << "\n";
  }

  return 0;
}
