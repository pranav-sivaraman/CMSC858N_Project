#include "KokkosRadixSort.hpp"

KokkosRadixSort::KokkosRadixSort(size_t num_digits, size_t num_values,
                                 std::vector<int> input)
    : RadixSort(num_digits, num_values),
      values((Kokkos::initialize(), "Values"), num_values),
      sorted_values("Sorted Values", num_values), counts("Counts", num_buckets),
      transpose_counts("Transpose Counts", num_buckets),
      offsets("Offsets", num_buckets),
      transpose_offsets("Transpose Offsets", num_buckets) {
  for (size_t i = 0; i < num_values; i++) {
    values(i) = input[i];
  }
}

KokkosRadixSort::~KokkosRadixSort() { Kokkos::finalize(); }

auto KokkosRadixSort::get_sorted_values() -> std::vector<int> {
  std::vector<int> result(num_values);
  for (size_t i = 0; i < num_values; i++) {
    result[i] = sorted_values(i);
  }

  return result;
}

auto KokkosRadixSort::get_key(size_t index) -> size_t {
  return (values(index) / pow10[iteration]) % 10;
}

void KokkosRadixSort::calculate_counts() {
  Kokkos::parallel_for(
      num_blocks, KOKKOS_LAMBDA(const size_t block_number) {
        size_t start = block_number * block_size;
        size_t end = std::min((block_number + 1) * block_size, num_values);

        for (size_t i = start; i < end; i++) {
          size_t key = get_key(i);
          counts(key + block_number * NUM_KEYS)++;
        }
      });
}

void KokkosRadixSort::calculate_offsets() {
  transpose(counts, transpose_counts, num_blocks, NUM_KEYS);
  scan();
  transpose(transpose_offsets, offsets, NUM_KEYS, num_blocks);
}

void KokkosRadixSort::place_values() {
  Kokkos::parallel_for(
      num_blocks, KOKKOS_LAMBDA(const size_t block_number) {
        size_t start = block_number * block_size;
        size_t end = std::min((block_number + 1) * block_size, num_values);

        for (size_t i = start; i < end; i++) {
          size_t key = get_key(i);
          sorted_values(offsets(key + block_number * NUM_KEYS)++) = values(i);
        }
      });
}

void KokkosRadixSort::reset() {
  Kokkos::parallel_for(
      num_buckets, KOKKOS_LAMBDA(const size_t i) { counts(i) = 0; });

  std::swap(values, sorted_values);
}

void KokkosRadixSort::scan() {
  Kokkos::parallel_scan(
      num_buckets,
      KOKKOS_LAMBDA(int64_t i, int64_t & partial_sum, bool is_final) {
        if (is_final) {
          transpose_offsets(i) = partial_sum;
        }
        partial_sum += transpose_counts(i);
      });
}

void KokkosRadixSort::transpose(const Kokkos::View<int *> &input,
                                Kokkos::View<int *> &output, size_t num_rows,
                                size_t num_cols) {
  Kokkos::parallel_for(
      num_buckets, KOKKOS_LAMBDA(const size_t i) {
        size_t row = i / num_rows;
        size_t col = i % num_rows;
        output(i) = input(col * num_cols + row);
      });
}
