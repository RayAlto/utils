cmake_minimum_required(VERSION 3.1)
project(
  ra-utils
  VERSION 1.0
  LANGUAGES CXX)
option(BUILD_TESTS "Build tests" ON)
set(RA_UTILS_SRCS
  src/exceptions.cpp
  src/subprocess.cpp)
set(RA_UTILS_INCLUDES
  ${CMAKE_CURRENT_LIST_DIR}/include/)
find_package(Threads REQUIRED)
add_library(ra-utils STATIC ${RA_UTILS_SRCS})
target_include_directories(ra-utils PRIVATE ${RA_UTILS_INCLUDES})
target_link_libraries(ra-utils PRIVATE Threads::Threads)

if(BUILD_TESTS)
  add_subdirectory(test)
endif()