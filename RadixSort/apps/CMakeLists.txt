add_executable(app app.cpp)
set_target_properties(app PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                     "${CMAKE_BINARY_DIR}")
target_compile_features(app PRIVATE cxx_std_17)
target_link_libraries(app PRIVATE serial_radix omp_radix kokkos_radix
                                  raja_radix sycl_radix)

install(TARGETS app)
