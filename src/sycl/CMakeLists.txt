set(OpenSYCL_DIR
    ${CMAKE_SOURCE_DIR}/external/sycl/sycl_install/lib/cmake/OpenSYCL)

find_package(OpenSYCL CONFIG REQUIRED)

add_library(sycl_radix SYCLRadixSort.cpp SYCLRadixSort.hpp)
target_include_directories(sycl_radix PUBLIC . ..)
add_sycl_to_target(TARGET sycl_radix SOURCES SYCLRadixSort.cpp
                   SYCLRadixSort.hpp)
