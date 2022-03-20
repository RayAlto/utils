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
  ${CMAKE_CURRENT_LIST_DIR}/src/request/mime_part.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/request/mime_parts.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/request/proxy.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/request/request.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/string/strtool.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/subprocess/subprocess.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/util/mime_types.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/util/mime_types_data.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/websocket/ws_client.cc
)
set(RA_UTILS_INCLUDES
  ${CMAKE_CURRENT_LIST_DIR}/include/)

find_package(Threads REQUIRED)
find_package(CURL COMPONENTS HTTP HTTPS SSL)
find_package(libwebsockets REQUIRED)

add_library(${CMAKE_PROJECT_NAME} STATIC ${RA_UTILS_SRCS})
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 11)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${RA_UTILS_INCLUDES})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE CURL::libcurl)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE websockets)

if(BUILD_TESTS)
  add_subdirectory(test)
endif()
