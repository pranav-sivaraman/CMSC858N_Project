add_library(serial_radix SerialRadixSort.cpp SerialRadixSort.hpp)
target_compile_features(serial_radix PUBLIC cxx_std_17)
target_include_directories(serial_radix PUBLIC .. .)
