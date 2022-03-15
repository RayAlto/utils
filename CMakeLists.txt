cmake_minimum_required(VERSION 3.1)
project(
  ra-utils
  VERSION 1.0
  LANGUAGES CXX)
option(BUILD_TESTS "Build tests" ON)
set(RA_UTILS_SRCS
  ${CMAKE_CURRENT_LIST_DIR}/src/exceptions/exceptions.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/request/authentication.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/request/cookie.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/request/header.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/request/proxy.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/request/request.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/string/strtool.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/subprocess/subprocess.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/util/mime_types.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/util/mime_types_data.cc
)
set(RA_UTILS_INCLUDES
  ${CMAKE_CURRENT_LIST_DIR}/include/)

find_package(Threads REQUIRED)
find_package(CURL COMPONENTS HTTP HTTPS SSL)

add_library(ra-utils STATIC ${RA_UTILS_SRCS})
set_property(TARGET ra-utils PROPERTY CXX_STANDARD 11)
target_include_directories(ra-utils PRIVATE ${RA_UTILS_INCLUDES})
target_link_libraries(ra-utils PRIVATE Threads::Threads)
target_link_libraries(ra-utils PRIVATE CURL::libcurl)

if(BUILD_TESTS)
  add_subdirectory(test)
endif()
