#!/usr/bin/env bash

cmake --build build/
ln -s build/raja/bin/app ./app
