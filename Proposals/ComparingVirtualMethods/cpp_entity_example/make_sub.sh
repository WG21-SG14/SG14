#!/bin/bash
touch CMakeLists.txt
# export LDFLAGS=-L/usr/local/opt/isl014/lib
# export CPPFLAGS=-I/usr/local/opt/isl014/include
# mkdir optdebug
# pushd optdebug
# cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -G"Sublime Text 2 - Unix Makefiles"  ..
# subl --project Main.sublime-project
# popd

mkdir release
pushd release
cmake -DCMAKE_BUILD_TYPE=Release -G"Sublime Text 2 - Unix Makefiles"  ..
subl --project Main.sublime-project
popd
# cmake -G"Sublime Text 2 - Unix Makefiles"  ..
