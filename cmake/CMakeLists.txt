cmake_minimum_required(VERSION 2.8.4)
project(sg14)

if(MSVC)
set(WARNING_FLAGS "/W4 /WX")
else()
set(WARNING_FLAGS "-Wall -Wextra -Wfatal-errors -Werror")
endif()

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} -D_SCL_SECURE_NO_WARNINGS")
else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -fno-rtti -Ofast")
set(CMAKE_EXE_LINKER_FLAGS "-g")
endif()

set(SG14_SOURCE_DIRECTORY "../SG14")
set(SG14_TEST_SOURCE_DIRECTORY "../SG14_test")

find_package(Threads REQUIRED)

set(SOURCE_FILES
    ${SG14_TEST_SOURCE_DIRECTORY}/main.cpp
    ${SG14_TEST_SOURCE_DIRECTORY}/SG14_test.cpp
    ${SG14_TEST_SOURCE_DIRECTORY}/unstable_remove_test.cpp
    ${SG14_TEST_SOURCE_DIRECTORY}/plf_colony_test_suite.cpp
    ${SG14_TEST_SOURCE_DIRECTORY}/plf_stack_test_suite.cpp
    ${SG14_TEST_SOURCE_DIRECTORY}/inplace_function_test.cpp
    ${SG14_TEST_SOURCE_DIRECTORY}/ring_test.cpp
    ${SG14_TEST_SOURCE_DIRECTORY}/transcode_test.cpp
    ${SG14_TEST_SOURCE_DIRECTORY}/uninitialized.cpp)

if(MSVC)
set(SG14_IGNORE_UNUSED_PARAMETER "/wd4100")
else()
set(SG14_IGNORE_UNUSED_PARAMETER "-Wno-unused-parameter")
endif()
set_source_files_properties(${SG14_TEST_SOURCE_DIRECTORY}/plf_stack_test_suite.cpp PROPERTIES
	COMPILE_FLAGS ${SG14_IGNORE_UNUSED_PARAMETER}
)
set_source_files_properties(${SG14_TEST_SOURCE_DIRECTORY}/plf_colony_test_suite.cpp PROPERTIES
	COMPILE_FLAGS ${SG14_IGNORE_UNUSED_PARAMETER}
)

add_executable(sg14 ${SOURCE_FILES})

include_directories("${SG14_SOURCE_DIRECTORY}" "${SG14_TEST_SOURCE_DIRECTORY}")

target_link_libraries(sg14 "${CMAKE_THREAD_LIBS_INIT}")
# "dl" "stdc++" "m")

