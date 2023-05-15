cmake_minimum_required(VERSION 3.19)

project(
  RadixSort
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

include(ExternalProject)

set_directory_properties(PROPERTIES EP_BASE ${CMAKE_BINARY_DIR}/ExtProjects)
get_directory_property(EXT_BASE_PATH EP_BASE)

set(EXT_INSTALL_PATH ${EXT_BASE_PATH}/Install)

include(ExternalOpenSYCL)
include(RadixSort)
