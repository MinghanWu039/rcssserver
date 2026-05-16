cmake_minimum_required(VERSION 3.15...4.3)

project(RCSSServer VERSION 19.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
  set(HAVE_LIBZ TRUE)
endif()
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
# Homebrew's Boost 1.90 package config does not ship a matching
# `boost_system` config package, so keep using FindBoost on newer CMake.
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 OLD)
endif()
set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost 1.44.0 REQUIRED)

set(RCSSSERVER_FLEX_INCLUDE_DIRS)
if(FLEX_INCLUDE_DIRS)
  list(APPEND RCSSSERVER_FLEX_INCLUDE_DIRS ${FLEX_INCLUDE_DIRS})
endif()
if(FLEX_INCLUDE_DIR)
  list(APPEND RCSSSERVER_FLEX_INCLUDE_DIRS ${FLEX_INCLUDE_DIR})
endif()
if(NOT RCSSSERVER_FLEX_INCLUDE_DIRS AND FLEX_EXECUTABLE)
  get_filename_component(RCSSSERVER_FLEX_BIN_DIR "${FLEX_EXECUTABLE}" DIRECTORY)
  get_filename_component(RCSSSERVER_FLEX_PREFIX "${RCSSSERVER_FLEX_BIN_DIR}" DIRECTORY)
  if(EXISTS "${RCSSSERVER_FLEX_PREFIX}/include/FlexLexer.h")
    list(APPEND RCSSSERVER_FLEX_INCLUDE_DIRS "${RCSSSERVER_FLEX_PREFIX}/include")
  endif()
endif()
list(REMOVE_DUPLICATES RCSSSERVER_FLEX_INCLUDE_DIRS)

add_library(RCSSFlex INTERFACE)
add_library(RCSS::Flex ALIAS RCSSFlex)
if(RCSSSERVER_FLEX_INCLUDE_DIRS)
  target_include_directories(RCSSFlex INTERFACE ${RCSSSERVER_FLEX_INCLUDE_DIRS})
endif()

include(GNUInstallDirs)
include(CheckIncludeFileCXX)

check_include_file_cxx("sys/socket.h" HAVE_SYS_SOCKET_H)
check_include_file_cxx("sys/param.h" HAVE_SYS_PARAM_H)
check_include_file_cxx("sys/time.h" HAVE_SYS_TIME_H)
check_include_file_cxx("netinet/in.h" HAVE_NETINET_IN_H)
check_include_file_cxx("arpa/inet.h" HAVE_ARPA_INET_H)
check_include_file_cxx("netdb.h" HAVE_NETDB_H)
check_include_file_cxx("unistd.h" HAVE_UNISTD_H)
check_include_file_cxx("poll.h" HAVE_POLL_H)
check_include_file_cxx("pwd.h" HAVE_PWD_H)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake config.h)

add_subdirectory(rcss)
add_subdirectory(src)
