#include <CL/sycl.hpp>

using T = float;

namespace sycl = cl::sycl;

constexpr size_t n = 1638400;

int main() {
  std::vector<T> A(n);
  std::vector<T> B(n);

  for (auto i = 0; i < n; ++i) {
    A[i] = i;
  }

  sycl::queue q{};
  std::cout << "Running on device: "
            << q.get_device().get_info<sycl::info::device::name>() << std::endl;

  try {
    sycl::buffer bufA{A};
    sycl::buffer bufB{B};

    q.submit([&](sycl::handler &cgh) {
       sycl::accessor accA{bufA, cgh};
       sycl::accessor accB{bufB, cgh};

       cgh.parallel_for(sycl::range{n},
                        [=](sycl::id<1> i) { accB[i] = accA[i] * accA[i]; });
     }).wait();
  } catch (sycl::exception &e) {
    std::cerr << "Caught exception: " << e.what();
  }

  std::cout << "Results: " << std::endl;
  std::for_each(B.begin(), B.begin() + 10, [](T &c) { std::cout << c << " "; });
  std::cout << "\n\n";
}
