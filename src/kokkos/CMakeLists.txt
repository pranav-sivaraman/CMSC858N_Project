FetchContent_Declare(
  kokkos
  GIT_REPOSITORY https://github.com/kokkos/kokkos
  GIT_TAG 4.0.01)

set(Kokkos_ENABLE_SERIAL
    OFF
    CACHE BOOL "")

set(Kokkos_ENABLE_OPENMP
    ON
    CACHE BOOL "")

FetchContent_MakeAvailable(kokkos)

add_library(kokkos_radix KokkosRadixSort.cpp KokkosRadixSort.hpp)
target_link_libraries(kokkos_radix Kokkos::kokkos)
target_include_directories(kokkos_radix PUBLIC . ..)
