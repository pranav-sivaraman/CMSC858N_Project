#!/usr/bin/env bash

CC=clang CXX=clang++ CMAKE_EXPORT_COMPILE_COMMANDS=ON cmake -S . -B build/
