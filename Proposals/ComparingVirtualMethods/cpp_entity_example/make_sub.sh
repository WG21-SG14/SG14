#!/bin/bash
touch CMakeLists.txt
# export LDFLAGS=-L/usr/local/opt/isl014/lib
# export CPPFLAGS=-I/usr/local/opt/isl014/include
mkdir out
cd out
cmake -G"Sublime Text 2 - Unix Makefiles" -DCMAKE_CXX_COMPILER=g++-5 -DCMAKE_CC_COMPILER=gcc-5 ..
# cmake -G"Sublime Text 2 - Unix Makefiles"  ..
subl --project Main.sublime-project
cd ..

