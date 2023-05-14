cmake_minimum_required(VERSION 3.19)

project(
  RadixSort
  VERSION 1.0
  LANGUAGES CXX)

include(FetchContent)

add_subdirectory(src)
add_subdirectory(apps)
