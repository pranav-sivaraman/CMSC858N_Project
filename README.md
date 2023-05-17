
# CMSC858N Project

Repository for CMSC858N Project

Radix Sort implemented in different parallel libraries




## Prerequisties
* CMake (3.19)
* Boost
* Compiler with OpenMP

If on MacOS you can get everything via Homebrew
```bash
  brew install llvm
  brew install boost
  brew install cmake
```
    
## Building

```bash
  git clone https://github.com/pranav-sivaraman/CMSC858N_Project.git
  cd CMSC858N_Project
  CC=clang CXX=clang++ cmake -S . -B build
  cmake --build build
  ln -s install_sort/bin/app .
```
    
## Running

Default problem size is 10. Default thread count is system amount. User has the option to change the number of threads and problem size

```bash
  export OMP_PROC_BIND=spread OMP_PLACES=threads
  OMP_NUM_THREADS=1 ./app N
```