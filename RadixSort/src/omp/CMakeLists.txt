add_library(omp_radix OMPRadixSort.cpp OMPRadixSort.hpp)
target_compile_features(omp_radix PUBLIC cxx_std_17)
target_include_directories(omp_radix PUBLIC . ..)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  target_link_libraries(omp_radix PUBLIC OpenMP::OpenMP_CXX)
endif()
