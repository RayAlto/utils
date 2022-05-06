cmake_minimum_required(VERSION 3.1)
project(
  ra-utils
  VERSION 1.0
  LANGUAGES CXX)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
option(BUILD_TESTS "Build tests" ON)
set(RA_UTILS_SRCS
  ${CMAKE_CURRENT_LIST_DIR}/src/crypto/codec.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/crypto/diffie_hellman.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/crypto/encrypt.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/crypto/hash.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/crypto/random.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/exceptions/exceptions.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/network/request.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/network/request/authentication.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/network/request/cookie.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/network/request/header.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/network/request/mime_part.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/network/request/mime_parts.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/network/request/proxy.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/network/ws_client.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/network/ws_client/close_status.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/string/strtool.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/misc/mime_types.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/misc/mime_types_data.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/db/sqlite.cc
)
set(RA_UTILS_INCLUDES
  ${CMAKE_CURRENT_LIST_DIR}/include/)

find_package(CURL REQUIRED COMPONENTS HTTP HTTPS SSL)
find_package(OpenSSL REQUIRED COMPONENTS Crypto SSL)
find_package(SQLite3 REQUIRED)
find_package(Threads REQUIRED)
find_package(websocketpp REQUIRED)

add_library(${CMAKE_PROJECT_NAME} STATIC ${RA_UTILS_SRCS})
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${RA_UTILS_INCLUDES})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE CURL::libcurl)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OpenSSL::Crypto)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OpenSSL::SSL)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE websocketpp::websocketpp)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE SQLite::SQLite3)

if(BUILD_TESTS)
  add_subdirectory(test)
endif()
