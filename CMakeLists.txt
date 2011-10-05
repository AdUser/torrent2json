SET(CNAME "json2torrent")
SET(VERSION 0.03)

PROJECT(${CNAME} C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# cmake modules
INCLUDE(CheckIncludeFile)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckFunctionExists)

IF    (CMAKE_BUILD_TYPE EQUAL "Debug")
  SET (CMAKE_C_FLAGS "-ggdb -O0 -Wall -pedantic -DDEBUG -std=c99")
ENDIF (CMAKE_BUILD_TYPE EQUAL "Debug")

# set paths
SET(CMAKE_INSTALL_PREFIX "/usr")
SET(INSTALL_BIN "${CMAKE_INSTALL_PREFIX}/bin")
SET(INSTALL_LIB "${CMAKE_INSTALL_PREFIX}/lib")

IF (NOT LIBDIR)
  SET (LIBDIR "lib")
ENDIF (NOT LIBDIR)

SET(REQUIRED_HEADERS
    "ctype.h" "stdbool.h" "stdio.h"
    "stdint.h" "stdlib.h" "string.h" "unistd.h")

FOREACH   (HDR ${REQUIRED_HEADERS})
  CHECK_INCLUDE_FILE (${HDR}  TEST_H)
  IF    (NOT TEST_H)
    MESSAGE(FATAL_ERROR "Build aborted.")
  ENDIF (NOT TEST_H)
  UNSET(TEST_H CACHE)
ENDFOREACH()

## checks
# yajl (yet another json library)
FIND_LIBRARY(LIB_YAJL yajl)

IF    (LIB_YAJL)
  SET (BUILD_LIBS ${BUILD_LIBS} ${LIB_YAJL})
  SET (YAJL_FOUND "FOUND")
ELSE  (LIB_YAJL)
  MESSAGE (SEND_ERROR "yajl not found")
ENDIF (LIB_YAJL)


MESSAGE (STATUS "------------------------------------------")
MESSAGE (STATUS "Build type is:        ${CMAKE_BUILD_TYPE}")
MESSAGE (STATUS "")
MESSAGE (STATUS "Install prefix:       ${CMAKE_INSTALL_PREFIX}")
MESSAGE (STATUS "Binaries directory:   ${INSTALL_BIN}")
MESSAGE (STATUS "")
MESSAGE (STATUS "Required libraries:")
MESSAGE (STATUS "  yajl: ${YAJL_FOUND}")
MESSAGE (STATUS "")
MESSAGE (STATUS "------------------------------------------")

ADD_SUBDIRECTORY (src)
