cmake_minimum_required (VERSION 2.6)
project (sitemon)

enable_language(CXX)

#SET(DEBUG_WARNINGS "-Wall -Wextra")
#SET(DEBUG_WARNINGS_SUPPRESS "-Wno-unused-function -Wno-unused-parameter -Wno-unused-variable")
#SET(CMAKE_CXX_FLAGS_DEBUG "-g -D_DEBUG -D_GLIBCXX_DEBUG ${DEBUG_WARNINGS} ${DEBUG_WARNINGS_SUPPRESS}")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -D_DEBUG ${DEBUG_WARNINGS} ${DEBUG_WARNINGS_SUPPRESS}")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -fvisibility-inlines-hidden -fvisibility=hidden")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING
	  "Choose the type of build, options are: None Debug Release"
	  FORCE)
endif(NOT CMAKE_BUILD_TYPE)
if(CMAKE_FIRST_RUN)
	message(STATUS "Setting Build Type to: ${CMAKE_BUILD_TYPE}")
endif()

FIND_PACKAGE(CURL REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(Threads)

include_directories(src "${CURL_INCLUDE_DIRS}")
FILE(GLOB_RECURSE sitemon_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")


ADD_EXECUTABLE(sitemon ${sitemon_SOURCES} "${CMAKE_SOURCE_DIR}/src/utils/sqlite3.c")

LINK_DIRECTORIES()
include_directories(${CMAKE_CURRENT_BINARY_DIR})

TARGET_LINK_LIBRARIES(sitemon ${CURL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${ZLIB_LIBRARY} dl )

