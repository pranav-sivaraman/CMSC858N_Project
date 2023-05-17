#include "SYCLRadixSort.hpp"

SYCLRadixSort::SYCLRadixSort(size_t num_digits, size_t num_values,
                             std::vector<int> input)
    : RadixSort(num_digits, num_values), values{input.cbegin(), input.cend()},
      sorted_values{num_values}, counts{num_buckets},
      transpose_counts{num_buckets}, offsets{num_buckets},
      transpose_offsets{num_buckets}, bpow10{pow10.cbegin(), pow10.cend()},
      partial_sums{num_blocks + 1} {
  queue = std::make_unique<sycl::queue>();
}

auto SYCLRadixSort::get_sorted_values() -> std::vector<int> {
  queue->wait();
  std::vector<int> v(num_values, 0);
  sycl::host_accessor _sorted_values{sorted_values, sycl::read_only};

  for (size_t i = 0; i < num_values; i++) {
    v[i] = _sorted_values[i];
  }
  return v;
}

auto SYCLRadixSort::get_key(size_t index) -> size_t { return 0; };

void SYCLRadixSort::calculate_counts() {
  size_t divisor = pow10[iteration];
  queue->submit([&](sycl::handler &cgh) {
    sycl::accessor kvalues{values, cgh, sycl::read_only};
    sycl::accessor kcounts{counts, cgh, sycl::read_write, sycl::no_init};

    cgh.parallel_for(sycl::range<1>{num_blocks}, [=](sycl::id<1> idx) {
      size_t block_number = idx[0];
      size_t start = block_number * block_size;
      size_t end = std::min((block_number + 1) * block_size, num_values);

      for (size_t i = start; i < end; i++) {
        size_t key = (kvalues[i] / divisor) % 10;
        kcounts[key + block_number * NUM_KEYS]++;
      }
    });
  });
}

void SYCLRadixSort::calculate_offsets() {
  transpose(counts, transpose_counts, num_blocks, NUM_KEYS);
  scan();
  transpose(transpose_offsets, offsets, NUM_KEYS, num_blocks);
}

void SYCLRadixSort::place_values() {
  size_t divisor = pow10[iteration];
  queue->submit([&](sycl::handler &cgh) {
    sycl::accessor kvalues{values, cgh, sycl::read_only};
    sycl::accessor ksorted_values{sorted_values, cgh, sycl::write_only,
                                  sycl::no_init};
    sycl::accessor koffsets{offsets, cgh, sycl::read_write};

    cgh.parallel_for(sycl::range<1>{num_blocks}, [=](sycl::id<1> idx) {
      size_t block_number = idx[0];
      size_t start = block_number * block_size;
      size_t end = std::min((block_number + 1) * block_size, num_values);

      for (size_t i = start; i < end; i++) {
        size_t key = (kvalues[i] / divisor) % 10;
        ksorted_values[koffsets[key + block_number * NUM_KEYS]++] = kvalues[i];
      }
    });
  });
}
void SYCLRadixSort::reset() {
  queue->submit([&](sycl::handler &cgh) {
    sycl::accessor kcounts{counts, cgh, sycl::write_only};

    cgh.parallel_for(sycl::range<1>{num_buckets},
                     [=](sycl::id<1> idx) { kcounts[idx] = 0; });
  });

  queue->wait();
  std::swap(values, sorted_values);
}

void SYCLRadixSort::scan() {
  queue->submit([&](sycl::handler &cgh) {
    sycl::accessor kdata{transpose_counts, cgh, sycl::read_only};
    sycl::accessor kprefix{transpose_offsets, cgh, sycl::write_only,
                           sycl::no_init};
    sycl::accessor kpartial{partial_sums, cgh, sycl::write_only, sycl::no_init};

    cgh.parallel_for(sycl::range<1>{num_blocks}, [=](sycl::id<1> idx) {
      int s = 0;
      size_t tid = idx[0];
      size_t start = tid * NUM_KEYS;
      size_t end = std::min((tid + 1) * NUM_KEYS, num_buckets);

      for (size_t i = start; i < end; i++) {
        kprefix[i] = s;
        s += kdata[i];
      }
      kpartial[tid + 1] = s;
    });
  });

  queue->submit([&](sycl::handler &cgh) {
    sycl::accessor kprefix{transpose_offsets, cgh, sycl::read_write};
    sycl::accessor kpartial{partial_sums, cgh, sycl::read_only};

    cgh.parallel_for(sycl::range<1>{num_blocks}, [=](sycl::id<1> idx) {
      int offset = 0;
      size_t tid = idx[0];
      for (size_t i = 0; i < tid + 1; i++) {
        offset += kpartial[i];
      }

      size_t start = tid * NUM_KEYS;
      size_t end = std::min((tid + 1) * NUM_KEYS, num_buckets);
      for (size_t i = start; i < end; i++) {
        kprefix[i] += offset;
      }
    });
  });
}

void SYCLRadixSort::transpose(sycl::buffer<int> &input,
                              sycl::buffer<int> &output, size_t num_rows,
                              size_t num_cols) {
  queue->submit([&](sycl::handler &cgh) {
    sycl::accessor kinput{input, cgh, sycl::read_only};
    sycl::accessor koutput{output, cgh, sycl::write_only, sycl::no_init};

    cgh.parallel_for(sycl::range<1>{num_buckets}, [=](sycl::id<1> idx) {
      size_t i = idx[0];
      size_t row = i / num_rows;
      size_t col = i % num_rows;
      koutput[i] = kinput[col * num_cols + row];
    });
  });
}
