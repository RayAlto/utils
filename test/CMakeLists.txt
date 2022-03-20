cmake_minimum_required(VERSION 3.1)
project(
  ra-utils-test
  VERSION 1.0
  LANGUAGES CXX)

macro(ra_test_add target_name target_src)
  add_executable(${target_name} ${target_src})
  target_include_directories(${target_name} PRIVATE ${RA_UTILS_INCLUDES})
  target_link_libraries(${target_name} PRIVATE ra-utils)
endmacro()

ra_test_add(subprocess test_subprocess.cc)
ra_test_add(timer test_timer.cc)
ra_test_add(request test_request.cc)
ra_test_add(strtool test_strtool.cc)
ra_test_add(mimetype test_mimetype.cc)
ra_test_add(wsclient test_websocket.cc)
