#include <iostream>
#include <random>

#include "OMPRadixSort.hpp"
#include "SerialRadixSort.hpp"

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

  std::vector<const std::string> model_names = {"Serial", "OpenMP"};
  RadixSort *models[] = {new SerialRadixSort(max_digits, N, input),
                         new OMPRadixSort(max_digits, N, input)};
  size_t num_models = model_names.size();

  for (size_t i = 1; i < num_models; i++) {
    std::cout << model_names[i] << "\n";
    models[i]->sort();
    std::vector<int> output = models[i]->get_sorted_values();

    if (std::is_sorted(output.cbegin(), output.cend())) {
      std::cout << "Data is sorted!\n";
    } else {
      std::cout << "Data is not sorted\n";
    }
  }

  return 0;
}
