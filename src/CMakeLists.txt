set(HEADER_LIST RadixSort.hpp)
set(SOURCE_LIST RadixSort.cpp)

add_library(models ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(models PUBLIC .)
