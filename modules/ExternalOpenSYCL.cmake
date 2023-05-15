set(OpenSYCL_ROOT
    ${EXT_INSTALL_PATH}/opensycl
    CACHE INTERNAL "")
ExternalProject_Add(
  opensycl
  URL https://github.com/OpenSYCL/OpenSYCL/archive/refs/tags/v0.9.4.zip
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
             -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
             -DCMAKE_INSTALL_PREFIX=${OpenSYCL_ROOT}
             -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
list(APPEND GLOBAL_THIRDPARTY_LIB_ARGS
     "-DhipSYCL_DIR:PATH=${OpenSYCL_ROOT}/lib/cmake/hipSYCL")
