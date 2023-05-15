find_package(hipSYCL CONFIG REQUIRED)

add_library(sycl_radix SYCLRadixSort.cpp SYCLRadixSort.hpp)
target_compile_features(sycl_radix PUBLIC cxx_std_17)
target_include_directories(sycl_radix PUBLIC . ..)
add_sycl_to_target(TARGET sycl_radix SOURCES SYCLRadixSort.cpp
                   SYCLRadixSort.hpp)
