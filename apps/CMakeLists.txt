add_executable(app app.cpp)
target_compile_features(app PRIVATE cxx_std_17)
target_link_libraries(app PRIVATE serial omp raja_radix kokkos_radix sycl_radix)
