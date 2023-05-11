add_library(omp OMPRadixSort.cpp OMPRadixSort.hpp)
target_compile_features(omp PUBLIC cxx_std_17)
target_include_directories(omp PUBLIC . ..)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  target_link_libraries(omp PUBLIC OpenMP::OpenMP_CXX)
endif()
