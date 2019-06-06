cmake_minimum_required(VERSION 3.10)
project(sg14 CXX)

find_package(Threads REQUIRED)

# Prefer C++17, downgrade if it isn't available.
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_STANDARD 17)

set(SG14_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/SG14")
set(SG14_TEST_SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/SG14_test")

# Output binary to predictable location.
set(BINARY_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
	string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR})
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib) # For future use with testing library.

##
# Project
##
file(GLOB_RECURSE INCLUDES "${SG14_INCLUDE_DIRECTORY}/*.hpp" "${SG14_INCLUDE_DIRECTORY}/*.h")
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${SG14_INCLUDE_DIRECTORY})
target_sources(${PROJECT_NAME} INTERFACE
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	$<BUILD_INTERFACE:${INCLUDES}>
)
target_include_directories(${PROJECT_NAME} INTERFACE
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	$<BUILD_INTERFACE:${SG14_INCLUDE_DIRECTORY}>
)

##
# Unit Tests
##
set(TEST_SOURCE_FILES
    ${SG14_TEST_SOURCE_DIRECTORY}/main.cpp
    ${SG14_TEST_SOURCE_DIRECTORY}/flat_map_test.cpp
    ${SG14_TEST_SOURCE_DIRECTORY}/flat_set_test.cpp
    ${SG14_TEST_SOURCE_DIRECTORY}/inplace_function_test.cpp
    ${SG14_TEST_SOURCE_DIRECTORY}/plf_colony_test.cpp
    ${SG14_TEST_SOURCE_DIRECTORY}/ring_test.cpp
    ${SG14_TEST_SOURCE_DIRECTORY}/slot_map_test.cpp
    ${SG14_TEST_SOURCE_DIRECTORY}/uninitialized_test.cpp
    ${SG14_TEST_SOURCE_DIRECTORY}/unstable_remove_test.cpp
)

set(TEST_NAME ${PROJECT_NAME}_tests)
add_executable(${TEST_NAME} ${TEST_SOURCE_FILES})
target_link_libraries(${TEST_NAME} ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(${TEST_NAME} PRIVATE "${SG14_TEST_SOURCE_DIRECTORY}")

# Compile options
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -Werror)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -Werror)
	set_source_files_properties(${SG14_TEST_SOURCE_DIRECTORY}/plf_colony_test.cpp PROPERTIES
		COMPILE_FLAGS "-Wno-unused-parameter"
	)
	if (CMAKE_CXX_COMPILER_VERSION MATCHES "^7.*")
		set_source_files_properties(${SG14_TEST_SOURCE_DIRECTORY}/slot_map_test.cpp PROPERTIES
			COMPILE_FLAGS "-Wno-error=unused-variable -Wno-error=unused-but-set-variable") # Fix gcc7 issues with structured bindings
		message("Disabled -Wunused-variable and -Wunused-but-set-variable for gcc ${CMAKE_CXX_COMPILER_VERSION}.")
	endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TEST_NAME})
	target_compile_options(${TEST_NAME} PRIVATE /Zc:__cplusplus /permissive- /W4 /WX)
	add_definitions(-DNOMINMAX -D_SCL_SECURE_NO_WARNINGS)
	set_source_files_properties(${SG14_TEST_SOURCE_DIRECTORY}/plf_colony_test.cpp PROPERTIES
		COMPILE_FLAGS "/wd4127") # Disable conditional expression is constant, use if constexpr
endif()
