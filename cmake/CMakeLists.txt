cmake_minimum_required(VERSION 2.8.4)
project(sg14)

set(WARNING_FLAGS "-Wall -Wextra -Wfatal-errors -Werror")
set(OPT_FLAGS "-fno-exceptions -fno-unwind-tables")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} ${OPT_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -fno-rtti -Ofast")
set(CMAKE_EXE_LINKER_FLAGS "-g")

set(SG14_SOURCE_DIRECTORY "../SG14")
set(SG14_TEST_SOURCE_DIRECTORY "../SG14_test")

set(SOURCE_FILES
    ${SG14_TEST_SOURCE_DIRECTORY}/main.cpp
    ${SG14_TEST_SOURCE_DIRECTORY}/SG14_test.cpp
    ${SG14_TEST_SOURCE_DIRECTORY}/fixed_point_test.cpp
    ${SG14_TEST_SOURCE_DIRECTORY}/unstable_remove_test.cpp)

add_executable(sg14 ${SOURCE_FILES})

include_directories("${SG14_SOURCE_DIRECTORY}" "${SG14_TEST_SOURCE_DIRECTORY}")

target_link_libraries(sg14)
# "dl" "pthread" "stdc++" "m")

