
# Cmake entry point
cmake_minimum_required (VERSION 2.8.9)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/rpavlik-cmake-modules-1c73e35/")
#message(STATUS $ENV{PTHREADS-WIN32}/Pre-built\.2/lib)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} $ENV{PTHREADS-WIN32}/Pre-built.2/lib/x86)
project (szthreadpool)

# Find pthread
find_library(PTHREADS_LIB pthreadVC2)

if(PTHREADS_LIB)
  message(STATUS ${PTHREADS_LIB})
endif()

# Setup handy variables:
set(SZTHREADPOOL_EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(SZTHREADPOOL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SZTHREADPOOL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Set platform-agnostic include dirs for the project
include_directories(
  ${SZTHREADPOOL_INCLUDE_DIR}
  ${TBB_INCLUDE_DIRS}
  $ENV{PTHREADS-WIN32}/Pre-built.2/include
)

# Set compiler definitions
add_definitions(${TBB_DEFINITIONS})

# Set headers and sources for the szutl library
set(SZTHREADPOOL_HEADERS
  ${SZTHREADPOOL_INCLUDE_DIR}/threadsafe_queue.h
  ${SZTHREADPOOL_INCLUDE_DIR}/thread_pool.h
  ${SZTHREADPOOL_INCLUDE_DIR}/join_threads.h
)
set(SZTHREADPOOL_SOURCES
  ${SZTHREADPOOL_SOURCE_DIR}/main.cpp
  ${SZTHREADPOOL_SOURCE_DIR}/thread_pool.cpp
)

# Create the szutl library
add_executable(szthpool ${SZTHREADPOOL_HEADERS} ${SZTHREADPOOL_SOURCES})

# Link target with libraries
target_link_libraries(szthpool
  ${PTHREADS_LIB}
)

# Set the output directories for the libraries and binaries
set_target_properties (szthpool
  PROPERTIES 
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CONFIGURED_ONCE TRUE CACHE INTERNAL
    "A flag showing that CMake has configured at least once.")

