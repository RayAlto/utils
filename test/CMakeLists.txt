cmake_minimum_required(VERSION 3.1)
project(
  ra-utils-test
  VERSION 1.0
  LANGUAGES CXX)
set(TEST_SRCS
  test.cpp)

add_executable(ra-utils-test ${TEST_SRCS})
target_include_directories(ra-utils-test PRIVATE ${RA_UTILS_INCLUDES})
target_link_libraries(ra-utils-test PRIVATE ra-utils)