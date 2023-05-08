set(RAJA_TREE "${CMAKE_SOURCE_DIR}/external/raja")
set(ENABLE_TESTS
    OFF
    CACHE BOOL "")
set(ENABLE_EXAMPLES
    OFF
    CACHE BOOL "")
set(ENABLE_REPRODUCERS
    OFF
    CACHE BOOL "")
set(ENABLE_EXERCISES
    OFF
    CACHE BOOL "")
set(ENABLE_DOCUMENTATION
    OFF
    CACHE BOOL "")
set(ENABLE_BENCHMARKS
    OFF
    CACHE BOOL "")
set(ENABLE_CUDA
    ${ENABLE_CUDA}
    CACHE BOOL "" FORCE)
set(ENABLE_OPENMP
    ON
    CACHE BOOL "")

add_subdirectory(${RAJA_TREE} ${CMAKE_BINARY_DIR}/raja)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(raja_radix RAJARadixSort.cpp RAJARadixSort.hpp)
target_compile_features(raja_radix PUBLIC cxx_std_17)
target_include_directories(raja_radix PUBLIC . ..)

target_link_libraries(raja_radix PUBLIC RAJA)
