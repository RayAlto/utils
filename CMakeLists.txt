cmake_minimum_required(VERSION 3.1)
project(
  ra-utils
  VERSION 1.0
  LANGUAGES CXX)
set(RA_UTILS_SRCS
  src/exceptions.cpp
  src/subprocess.cpp)
set(RA_UTILS_INCLUDES
  include/)
set(TEST_SRCS
  test/test.cpp)
add_library(ra-utils STATIC ${RA_UTILS_SRCS})
target_include_directories(ra-utils PRIVATE ${RA_UTILS_INCLUDES})

add_executable(ra-utils-test ${TEST_SRCS})
target_include_directories(ra-utils-test PRIVATE ${RA_UTILS_INCLUDES})
target_link_libraries(ra-utils-test PRIVATE ra-utils)