cmake_minimum_required(VERSION 3.10)

project(Mutator VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_COMPILER "clang++")
add_compile_options(-fPIC -W)

# lib dir
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
add_library(mutator SHARED mutator.cc)
add_library(knobs SHARED knobs.cc)

add_executable(mutator_test mutator_test.cc)

target_link_libraries(mutator_test mutator knobs)
