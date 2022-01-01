cmake_minimum_required (VERSION 3.0)
project (sitemon)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(QMAKE_CXXFLAGS "-std=c++11")

#set(DEBUG_WARNINGS "-Wall -Wextra")
#set(DEBUG_WARNINGS_SUPPRESS "-Wno-unused-function -Wno-unused-parameter -Wno-unused-variable")
#set(CMAKE_CXX_FLAGS_DEBUG "-g -D_DEBUG -D_GLIBCXX_DEBUG ${DEBUG_WARNINGS} ${DEBUG_WARNINGS_SUPPRESS}")
set(CMAKE_CXX_FLAGS_DEBUG "-g -D_DEBUG ${DEBUG_WARNINGS} ${DEBUG_WARNINGS_SUPPRESS}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -fvisibility-inlines-hidden -fvisibility=hidden")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING
	  "Choose the type of build, options are: None Debug Release"
	  FORCE)
endif(NOT CMAKE_BUILD_TYPE)
if(CMAKE_FIRST_RUN)
	message(STATUS "Setting Build Type to: ${CMAKE_BUILD_TYPE}")
endif()

find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Threads)

include_directories(src "${CURL_INCLUDE_DIRS}")
file(GLOB_RECURSE sitemon_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# statically link libc and libstdc++
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

add_executable(sitemon ${sitemon_SOURCES} "${CMAKE_SOURCE_DIR}/src/utils/sqlite3.c")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(sitemon ${CURL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${ZLIB_LIBRARY} dl)

