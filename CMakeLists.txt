cmake_minimum_required(VERSION 3.15)

project(rjob)

add_executable(rjob_test rjob.cpp rjob_test.cpp test_main.cpp)

target_include_directories(rjob_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external/Catch2/single_include)