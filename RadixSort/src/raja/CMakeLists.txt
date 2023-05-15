set(RAJA_ENABLE_TESTS
    OFF
    CACHE BOOL "")
set(RAJA_ENABLE_EXAMPLES
    OFF
    CACHE BOOL "")
set(RAJA_ENABLE_EXERCISES
    OFF
    CACHE BOOL "")
set(RAJA_ENABLE_DOCUMENTATION
    OFF
    CACHE BOOL "")
set(ENABLE_OPENMP
    ON
    CACHE BOOL "")

FetchContent_Declare(
  raja
  GIT_REPOSITORY https://github.com/LLNL/RAJA
  GIT_TAG v2022.10.5)

FetchContent_MakeAvailable(raja)

add_library(raja_radix RAJARadixSort.cpp RAJARadixSort.hpp)

target_compile_features(raja_radix PUBLIC cxx_std_17)
target_include_directories(raja_radix PUBLIC . ..)
target_link_libraries(raja_radix PUBLIC RAJA)
