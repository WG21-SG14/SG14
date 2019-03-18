# SG14
[![Build Status](https://travis-ci.org/WG21-SG14/SG14.svg?branch=master)](https://travis-ci.org/WG21-SG14/SG14)

A library for Study Group 14 of Working Group 21 (C++)

/docs - Documentation for implementations without proposals, or supplementary documentation which does not easily fit within a proposal.

/docs/proposals - C++ standard proposals.

/SG14 - Source files for implementations.

/SG14_test - Individual tests for implementations.

http://lists.isocpp.org/mailman/listinfo.cgi/sg14 for more information

## Build Instructions
Clone the repo. Navigate to the folder in your favorite terminal.

`mkdir build && cd build`

### Windows
`cmake .. -A x64 && cmake --build . && bin\sg14_tests.exe`

### Unixes
`cmake .. && cmake --build . && ./bin/sg14_tests`

### Alternatively
`cd SG14_test && g++ -std=c++14 -DTEST_MAIN -I../SG14 whatever_test.cpp && ./a.out`
