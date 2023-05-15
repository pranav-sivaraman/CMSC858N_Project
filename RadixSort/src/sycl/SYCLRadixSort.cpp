#include "SYCLRadixSort.hpp"
#include "RadixSort.hpp"
#include <cmath>
#include <hipSYCL/sycl/libkernel/accessor.hpp>
#include <numeric>

SYCLRadixSort::SYCLRadixSort(size_t num_digits, size_t num_values,
                             std::vector<int> input)
    : RadixSort(num_digits, num_values), values{num_values},
      sorted_values{num_values}, counts{num_buckets},
      transpose_counts{num_buckets}, offsets{num_buckets},
      transpose_offsets{num_buckets}, partial_sums{num_blocks}, bpow10{
                                                                    NUM_KEYS} {
  sycl::host_accessor kvalues{values, sycl::write_only, sycl::no_init};
  sycl::host_accessor kcounts{counts, sycl::write_only, sycl::no_init};
  sycl::host_accessor kpow10{bpow10, sycl::write_only, sycl::no_init};

  for (size_t i = 0; i < num_values; i++) {
    kvalues[i] = input[i];
  }
  for (size_t i = 0; i < num_buckets; i++) {
    kcounts[i] = 0;
  }
  for (size_t i = 0; i < NUM_KEYS; i++) {
    kpow10[i] = pow10[i];
  }
}

auto SYCLRadixSort::get_sorted_values() -> std::vector<int> {
  std::vector<int> v(num_values, 0);
  // sycl::host_accessor kvalues{sorted_values, sycl::read_only};

  // for (size_t i = 0; i < num_values; i++) {
  // v[i] = kvalues[i];
  // }

  return v;
}

auto SYCLRadixSort::get_key(size_t index) -> size_t { return 0; }

void SYCLRadixSort::calculate_counts() {
  q.submit([=](sycl::handler &cgh) {
    sycl::accessor kvalues{values, cgh, sycl::read_only};
    sycl::accessor kcounts{counts, cgh, sycl::read_write};
    sycl::accessor kpow10{bpow10, cgh, sycl::read_only};

    cgh.parallel_for(sycl::range<1>{num_blocks}, [=](sycl::id<1> block_number) {
      size_t start = block_number * block_size;
      size_t end =
          std::min(((size_t)block_number + 1) * block_size, num_values);

      for (size_t i = start; i < end; i++) {
        size_t key = (kvalues[i] / kpow10[iteration]) % 10;
        kcounts[key + block_number * NUM_KEYS]++;
      }
    });
  });
}

void SYCLRadixSort::calculate_offsets() {
  int num_rows = num_blocks;
  int num_cols = NUM_KEYS;
  q.submit([=](sycl::handler &cgh) {
    sycl::accessor kinput{counts, cgh, sycl::read_only};
    sycl::accessor koutput{transpose_counts, cgh, sycl::write_only,
                           sycl::no_init};

    cgh.parallel_for<class vector_add>(
        sycl::range{(size_t)num_buckets}, [=](sycl::id<1> i) {
          size_t row = i / num_rows;
          size_t col = i % num_rows;
          koutput[i] = kinput[col * num_cols + row];
        });
  });

  scan();

  num_rows = NUM_KEYS;
  num_cols = num_blocks;
  q.submit([=](sycl::handler &cgh) {
    sycl::accessor kinput{transpose_offsets, cgh, sycl::read_only};
    sycl::accessor koutput{offsets, cgh, sycl::write_only, sycl::no_init};

    cgh.parallel_for<class vector_add>(
        sycl::range{(size_t)num_buckets}, [=](sycl::id<1> i) {
          size_t row = i / num_rows;
          size_t col = i % num_rows;
          koutput[i] = kinput[col * num_cols + row];
        });
  });
}

void SYCLRadixSort::place_values() {
  q.submit([=](sycl::handler &cgh) {
    sycl::accessor kvalues{values, cgh, sycl::read_only};
    sycl::accessor koffsets{offsets, cgh};
    sycl::accessor ksorted_values{sorted_values, cgh};
    sycl::accessor kpow10{bpow10, cgh, sycl::read_only};

    cgh.parallel_for(sycl::range<1>{num_blocks}, [=](sycl::id<1> block_number) {
      size_t start = block_number * block_size;
      size_t end =
          std::min(((size_t)block_number + 1) * block_size, num_values);

      for (size_t i = start; i < end; i++) {
        size_t key = (kvalues[i] / kpow10[iteration]) % 10;
        ksorted_values[koffsets[key + block_number * NUM_KEYS]++] = kvalues[i];
      }
    });
  });
}

void SYCLRadixSort::reset() {
  q.submit([=](sycl::handler &cgh) {
    sycl::accessor kcounts{counts, cgh, sycl::write_only, sycl::no_init};

    cgh.parallel_for(sycl::range<1>{num_buckets},
                     [=](sycl::id<1> i) { kcounts[i] = 0; });
  });

  std::swap(sorted_values, values);
}

void SYCLRadixSort::scan() {
  q.submit([=](sycl::handler &cgh) {
    sycl::accessor accData{transpose_counts, cgh, sycl::read_only};
    sycl::accessor accPrefix{transpose_offsets, cgh, sycl::write_only,
                             sycl::no_init};
    sycl::accessor accPartial{partial_sums, cgh, sycl::write_only,
                              sycl::no_init};

    cgh.parallel_for<class vector_add>(
        sycl::range{(size_t)num_blocks}, [=](sycl::id<1> tid) {
          int s = 0;
          size_t start = tid * NUM_KEYS;
          size_t end = std::min(((size_t)tid + 1) * NUM_KEYS, num_buckets);
          for (size_t i = start; i < end; i++) {
            accPrefix[i] = s;
            s += accData[i];
          }
          accPartial[tid + 1] = s;
        });
  });

  q.submit([=](sycl::handler &cgh) {
    sycl::accessor accPrefix{transpose_offsets, cgh};
    sycl::accessor accPartial{partial_sums, cgh, sycl::read_only};

    cgh.parallel_for<class vector_add>(
        sycl::range{(size_t)num_blocks}, [=](sycl::id<1> tid) {
          int offset = 0;
          for (size_t i = 0; i < (size_t)tid + 1; i++) {
            offset += accPartial[i];
          }

          size_t start = tid * NUM_KEYS;
          size_t end =
              std::min(((size_t)tid + 1) * NUM_KEYS, (size_t)num_buckets);
          for (size_t i = start; i < end; i++) {
            accPrefix[i] += offset;
          }
        });
  });
}
