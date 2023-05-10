#!/usr/bin/env bash

brew install llvm
brew install boost
CC=clang CXX=clang++ cmake -S external/sycl/. -B external/sycl/build/ -DCMAKE_INSTALL_PREFIX=external/sycl/sycl_install/
cmake --build external/sycl/build/ --target install
CC=clang CXX=clang++ CMAKE_EXPORT_COMPILE_COMMANDS=ON cmake -S . -B build/
cmake --build build/
ln -s build/raja/bin/app .
