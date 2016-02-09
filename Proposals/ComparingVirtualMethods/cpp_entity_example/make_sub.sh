#!/bin/bash
touch CMakeLists.txt
# export LDFLAGS=-L/usr/local/opt/isl014/lib
# export CPPFLAGS=-I/usr/local/opt/isl014/include
mkdir out
cd out
cmake -G"Sublime Text 2 - Unix Makefiles"  ..
# cmake -G"Sublime Text 2 - Unix Makefiles"  ..
subl --project Main.sublime-project
cd ..

