cmake_minimum_required(VERSION 3.10)

project(Mutator VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_COMPILER "clang++")
add_compile_options(-fPIC -W)

# lib dir
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/test)

add_library(mutator SHARED mutator.cc)
add_library(knobs SHARED knobs.cc)


add_executable(mutator_test mutator_test.cc)
add_executable(knobs_test knobs_test.cc)

# enable sanitize coverage
include(./thook.cmake)
enable_thook(mutator_test)

target_link_libraries(mutator_test PRIVATE mutator knobs)
target_link_libraries(knobs_test knobs)


# enable_testing()
# add_test(NAME mutator_test COMMAND mutator_test)
# add_test(NAME knobs_test COMMAND knobs_test)
