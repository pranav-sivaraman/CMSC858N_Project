set(Kokkos_ENABLE_OPENMP
    ON
    CACHE BOOL "")

set(Kokkos_ENABLE_SERIAL
    OFF
    CACHE BOOL "")

# We build kokkos as a subdirectory of our project
add_subdirectory(${CMAKE_SOURCE_DIR}/external/kokkos ${CMAKE_BINARY_DIR}/kokkos)

add_library(kokkos_radix KokkosRadixSort.cpp KokkosRadixSort.hpp)

# This is the only line required to set up all compiler/linker flags
target_link_libraries(kokkos_radix Kokkos::kokkos)

target_include_directories(kokkos_radix PUBLIC . ..)
