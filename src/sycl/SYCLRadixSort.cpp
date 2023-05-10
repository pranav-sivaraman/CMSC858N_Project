#include "SYCLRadixSort.hpp"

#include <cmath>
#include <iostream>

SYCLRadixSort::SYCLRadixSort(std::vector<int> values,
                             std::vector<int> sorted_values, int num_digits,
                             int num_values)
    : values(std::move(values)), sorted_values(std::move(sorted_values)) {
  this->num_digits = num_digits;
  this->num_values = num_values;
  this->block_length = num_digits / NUM_BUCKETS;
  if (this->block_length == 0) {
    block_length = 1;
  }
  this->num_blocks = std::ceil(num_values / block_length);

  offsets = std::vector<int>(num_blocks * NUM_BUCKETS);
  counts = std::vector<int>(num_blocks * NUM_BUCKETS);
  transpose_offsets = std::vector<int>(num_blocks * NUM_BUCKETS);
  transpose_counts = std::vector<int>(num_blocks * NUM_BUCKETS);
}

auto SYCLRadixSort::get_sorted_values() -> std::vector<int> {
  return sorted_values;
}

auto SYCLRadixSort::get_key(int index) -> int {
  return (values[index] / this->pow10[iteration]) % 10;
}

void SYCLRadixSort::calculate_counts() {
  cl::sycl::buffer bufValues{values};
  cl::sycl::buffer bufCounts{counts};
  cl::sycl::buffer bufPow10{pow10};

  q.submit([&](auto &cgh) {
    cl::sycl::accessor accValues{bufValues, cgh};
    cl::sycl::accessor accCounts{bufCounts, cgh};
    cl::sycl::accessor accPow10{bufPow10, cgh};

    cgh.parallel_for(
        cl::sycl::range{(size_t)num_blocks}, [=](cl::sycl::id<1> idx) {
          int i = (int)idx[0];
          size_t block_start = i * block_length;
          size_t block_end = std::min((i + 1) * block_length, num_values);

          for (int k = block_start; k < block_end; k++) {
            int key = (accValues[k] / accPow10[iteration]) % 10;
            accCounts[key + i * NUM_BUCKETS]++;
          }
        });
  });
}

void SYCLRadixSort::calculate_offsets() {
  std::vector<int> partial_sums(num_blocks + 1, 0);

  cl::sycl::buffer dataBuf{transpose_counts};
  cl::sycl::buffer prefixBuf{transpose_offsets};
  cl::sycl::buffer partialBuf{partial_sums};

  q.submit([&](cl::sycl::handler &cgh) {
    cl::sycl::accessor accData{dataBuf, cgh};
    cl::sycl::accessor accPrefix{prefixBuf, cgh};
    cl::sycl::accessor accPartial{partialBuf, cgh};
    cgh.parallel_for<class vector_add>(
        cl::sycl::range{(size_t)num_blocks}, [=](cl::sycl::id<1> tid) {
          int s = 0;
          size_t start = tid * NUM_BUCKETS;
          size_t end = std::min(((size_t)tid + 1) * NUM_BUCKETS,
                                (size_t)num_blocks * NUM_BUCKETS);
          for (size_t i = start; i < end; i++) {
            accPrefix[i] = s;
            s += accData[i];
          }
          accPartial[tid + 1] = s;
        });
  });

  q.submit([&](cl::sycl::handler &cgh) {
    cl::sycl::accessor accPrefix{prefixBuf, cgh};
    cl::sycl::accessor accPartial{partialBuf, cgh};

    cgh.parallel_for<class vector_add>(
        cl::sycl::range{(size_t)num_blocks}, [=](cl::sycl::id<1> tid) {
          int offset = 0;
          for (int i = 0; i < tid + 1; i++) {
            offset += accPartial[i];
          }

          size_t start = tid * NUM_BUCKETS;
          size_t end = std::min(((size_t)tid + 1) * NUM_BUCKETS,
                                (size_t)num_blocks * NUM_BUCKETS);
          for (size_t i = start; i < end; i++) {
            accPrefix[i] += offset;
          }
        });
  });
}

void SYCLRadixSort::place_values() {
  cl::sycl::buffer bufValues{values};
  cl::sycl::buffer bufSortedValues{sorted_values};
  cl::sycl::buffer bufOffsets{offsets};
  cl::sycl::buffer bufPow10{pow10};

  q.submit([&](cl::sycl::handler &cgh) {
    cl::sycl::accessor accValues{bufValues, cgh};
    cl::sycl::accessor accSortedValues{bufSortedValues, cgh};
    cl::sycl::accessor accOffsets{bufOffsets, cgh};
    cl::sycl::accessor accPow10{bufPow10, cgh};

    cgh.parallel_for(
        cl::sycl::range{(size_t)num_blocks}, [=](cl::sycl::id<1> tid) {
          size_t start = tid * block_length;
          size_t end =
              std::min((size_t)(tid + 1) * block_length, (size_t)num_values);

          for (size_t k = start; k < end; k++) {
            int key = (accValues[k] / accPow10[iteration]) % 10;
            accSortedValues[accOffsets[key + tid * NUM_BUCKETS]++] = values[k];
          }
        });
  });
}

void SYCLRadixSort::transpose(const std::vector<int> &input,
                              std::vector<int> &output, int M, int N) {

  cl::sycl::buffer bufInput{input};
  cl::sycl::buffer bufOutput{output};

  q.submit([&](auto &cgh) {
    cl::sycl::accessor accInput{bufInput, cgh};
    cl::sycl::accessor accOutput{bufOutput, cgh};

    cgh.parallel_for(cl::sycl::range{(size_t)M * N}, [=](cl::sycl::id<1> idx) {
      int i = (int)idx[0];
      int row = i / M;
      int col = i % M;

      accOutput[i] = accInput[col * N + row];
    });
  });
}

void SYCLRadixSort::sort() {

  for (; iteration < num_digits; iteration++) {

    calculate_counts();

    transpose(counts, transpose_counts, num_blocks, NUM_BUCKETS);
    calculate_offsets();
    transpose(transpose_offsets, offsets, NUM_BUCKETS, num_blocks);

    place_values();

    if (iteration != num_digits - 1) {
      cl::sycl::buffer bufCounts{counts};

      q.submit([&](auto &cgh) {
        cl::sycl::accessor accCounts{bufCounts, cgh};

        cgh.parallel_for(cl::sycl::range{(size_t)num_blocks * NUM_BUCKETS},
                         [=](cl::sycl::id<1> idx) { accCounts[idx] = 0; });
      });
      std::swap(values, sorted_values);
    }
  }
}
