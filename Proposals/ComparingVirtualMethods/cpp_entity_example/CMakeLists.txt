cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project (Main CXX)

if(MSVC)
endif()

set (CMAKE_CXX_STANDARD 11)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-pmf-conversions" ) 
	# add_definitions("-Wno-c++11-extensions")
endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

# http://cognitivewaves.wordpress.com/cmake-and-visual-studio/
# http://www.cmake.org/Wiki/CMake_Useful_Variables
# add the executable
FILE(GLOB SRCFILES *.cpp *.h)
add_executable (Main ${SRCFILES})
