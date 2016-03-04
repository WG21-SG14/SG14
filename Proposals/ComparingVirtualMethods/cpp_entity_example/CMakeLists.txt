cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project (Main CXX)

if(MSVC)
endif()

set (CMAKE_CXX_STANDARD 11)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-pmf-conversions" ) 
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3 -Wall")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -Wall")	
endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

# http://cognitivewaves.wordpress.com/cmake-and-visual-studio/
# http://www.cmake.org/Wiki/CMake_Useful_Variables
# add the executable
FILE(GLOB SRCFILES *.cpp *.h)
add_executable (Main ${SRCFILES})
