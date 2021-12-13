cmake_minimum_required(VERSION 3.1)
project(
  ra-utils-test
  VERSION 1.0
  LANGUAGES CXX)

function(ra_add_test target_name target_src)
  add_executable(${target_name} ${target_src})
  target_include_directories(${target_name} PRIVATE ${RA_UTILS_INCLUDES})
  target_link_libraries(${target_name} PRIVATE ra-utils)
endfunction()

ra_add_test(subprocess test_subprocess.cpp)
ra_add_test(timer test_timer.cpp)
