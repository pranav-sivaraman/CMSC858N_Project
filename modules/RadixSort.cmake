ExternalProject_Add(
  radix_sort
  DEPENDS opensycl
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/RadixSort
  CMAKE_ARGS ${GLOBAL_THIRDPARTY_LIB_ARGS} -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
             -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/install_sort
             -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
