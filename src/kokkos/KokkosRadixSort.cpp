#include "KokkosRadixSort.hpp"
#include "RadixSort.hpp"
#include <cmath>

KokkosRadixSort::KokkosRadixSort(std::vector<int> values,
                                 std::vector<int> sorted_values, int num_digits,
                                 int num_values) {
  Kokkos::initialize();
  this->num_digits = num_digits;
  this->num_values = num_values;
  this->block_length = num_digits / NUM_BUCKETS;
  if (this->block_length == 0) {
    block_length = 1;
  }
  this->num_blocks = std::ceil(num_values / block_length);

  this->values = new Kokkos::View<int *>("values", num_values);
  this->sorted_values = new Kokkos::View<int *>("sorted_values", num_values);

  Kokkos::parallel_for(
      num_values,
      KOKKOS_LAMBDA(const int i) { (*this->values)(i) = values[i]; });

  this->offsets = new Kokkos::View<int *>("offsets", num_blocks * NUM_BUCKETS);
  this->counts = new Kokkos::View<int *>("counts", num_blocks * NUM_BUCKETS);
  this->transpose_offsets =
      new Kokkos::View<int *>("transpose_offsets", num_blocks * NUM_BUCKETS);
  this->transpose_counts =
      new Kokkos::View<int *>("transpose_counts", num_blocks * NUM_BUCKETS);

  Kokkos::fence();
}

KokkosRadixSort::~KokkosRadixSort() { Kokkos::finalize(); }

auto KokkosRadixSort::get_sorted_values() -> std::vector<int> {
  std::vector<int> result(num_values, 0);
  for (int i = 0; i < num_values; i++) {
    result[i] = (*sorted_values)(i);
  }
  return result;
}

auto KokkosRadixSort::get_key(int index) -> int {
  return ((*values)(index) / this->pow10[iteration]) % 10;
}

void KokkosRadixSort::calculate_counts(int block_number, int start_index,
                                       int end_index) {
  for (int i = start_index; i < end_index; i++) {
    int key = get_key(i);
    (*counts)(key + block_number * NUM_BUCKETS)++;
  }
}

void KokkosRadixSort::calculate_offsets() {
  Kokkos::parallel_scan(
      "Loop1", num_blocks * NUM_BUCKETS,
      KOKKOS_LAMBDA(int64_t i, int64_t & partial_sum, bool is_final) {
        if (is_final)
          (*transpose_offsets)(i) = partial_sum;
        partial_sum += (*transpose_counts)(i);
      });
}

void KokkosRadixSort::place_values(int block_number, int start_index,
                                   int end_index) {
  for (int i = start_index; i < end_index; i++) {
    int key = get_key(i);
    (*sorted_values)((*offsets)(key + block_number * NUM_BUCKETS)++) =
        (*values)(i);
  }
}

void KokkosRadixSort::transpose(const Kokkos::View<int *> &input,
                                Kokkos::View<int *> &output, int M, int N) {
  Kokkos::parallel_for(
      M * N, KOKKOS_LAMBDA(const int i) {
        int row = i / M;
        int col = i % M;
        output(i) = input(col * N + row);
      });

  Kokkos::fence();
}

void KokkosRadixSort::sort() {
  for (; iteration < num_digits; iteration++) {
    Kokkos::parallel_for(
        num_blocks, KOKKOS_LAMBDA(const int i) {
          int block_start = i * block_length;
          int block_end = std::min((i + 1) * block_length, num_values);
          calculate_counts(i, block_start, block_end);
        });

    Kokkos::fence();

    transpose(*counts, *transpose_counts, num_blocks, NUM_BUCKETS);
    calculate_offsets();
    transpose(*transpose_offsets, *offsets, NUM_BUCKETS, num_blocks);

    Kokkos::parallel_for(
        num_blocks, KOKKOS_LAMBDA(const int i) {
          int block_start = i * block_length;
          int block_end = std::min((i + 1) * block_length, num_values);
          place_values(i, block_start, block_end);
        });

    Kokkos::fence();

    if (iteration != num_digits - 1) {
      Kokkos::parallel_for(
          num_blocks * NUM_BUCKETS, KOKKOS_LAMBDA(const int i) {
            (*offsets)(i) = 0;
            (*transpose_offsets)(i) = 0;
            (*counts)(i) = 0;
            (*transpose_counts)(i) = 0;
          });
      std::swap(values, sorted_values);
    }

    Kokkos::fence();
  }
}
