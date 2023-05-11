add_library(serial SerialRadixSort.cpp SerialRadixSort.hpp)
target_compile_features(serial PUBLIC cxx_std_17)
target_include_directories(serial PUBLIC .. .)
