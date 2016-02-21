# Usage:
# use "set(SZTHREADPOOL_BUILD_EXE TRUE/FALSE)" in the parent scope to decide whether to build
# or not the executable or the library; true = build exe, false = build library

# Cmake entry point
cmake_minimum_required (VERSION 2.8.9)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/rpavlik-cmake-modules-1c73e35/")
#message(STATUS $ENV{PTHREADS-WIN32}/Pre-built\.2/lib)
if(${CMAKE_SYSTEM_NAME} matches "Windows")
  set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} $ENV{PTHREADS-WIN32}/Pre-built.2/lib/x86)
  message(STATUS "Windows detected")
  find_library(PTHREADS_LIB pthreadVC2 REQUIRED)
  if(PTHREADS_LIB)
    message(STATUS ${PTHREADS_LIB})
  endif()
elseif(${CMAKE_SYSTEM_NAME} matches "Linux")
  message(STATUS "Linux detected")
  find_package(Threads REQUIRED)
  if(THREADS_HAVE_PTHREAD_ARG)
    set(COMPILE_OPTS "-pthread")
  endif()
  if(CMAKE_THREAD_LIBS_INIT)
    set(PTHREADS_LIB "${CMAKE_THREAD_LIBS_INIT}")
  endif()
endif(${CMAKE_SYSTEM_NAME} matches "Linux")

project (szthreadpool)

# Find pthread

# Setup handy variables:
set(SZTHREADPOOL_EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(SZTHREADPOOL_EXTERNAL_DIR ${SZTHREADPOOL_EXTERNAL_DIR} PARENT_SCOPE)
set(SZTHREADPOOL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SZTHREADPOOL_INCLUDE_DIR ${SZTHREADPOOL_INCLUDE_DIR} PARENT_SCOPE)
set(SZTHREADPOOL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SZTHREADPOOL_SOURCE_DIR ${SZTHREADPOOL_SOURCE_DIR} PARENT_SCOPE)

# Set include dirs for the project
include_directories(
  ${SZTHREADPOOL_INCLUDE_DIR}
  ${TBB_INCLUDE_DIRS}
  $ENV{PTHREADS-WIN32}/Pre-built.2/include
)

# Set headers and sources for the szutl library
set(SZTHREADPOOL_HEADERS
  ${SZTHREADPOOL_INCLUDE_DIR}/thread_pool_c.h
  ${SZTHREADPOOL_INCLUDE_DIR}/dbg.h
)
set(SZTHREADPOOL_SOURCES_EXECUTABLE
  ${SZTHREADPOOL_SOURCE_DIR}/main.cpp
  ${SZTHREADPOOL_SOURCE_DIR}/thread_pool_c.c
)
set(SZTHREADPOOL_SOURCES_LIB
  ${SZTHREADPOOL_SOURCE_DIR}/thread_pool_c.c
)

# Create the executable
if(${SZTHREADPOOL_BUILD_EXE})
  add_executable(szthpool ${SZTHREADPOOL_HEADERS}
    ${SZTHREADPOOL_SOURCES_EXECUTABLE})
  # Link target (the executable) with libraries
  target_link_libraries(szthpool
    ${PTHREADS_LIB}
  )
  # Set the output directories for the libraries and binaries
  set_target_properties (szthpool
    PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  # Set compile options if on linux
  set_property(TARGET szthpool PROPERTY COMPILE_OPTIONS ${COMPILE_OPTS})
  set_property(TARGET szthpool PROPERTY INTERFACE_COMPILE_OPTIONS ${COMPILE_OPTS})
else()
  # Create the library version
  add_library(libszthpool ${SZTHREADPOOL_HEADERS}
    ${SZTHREADPOOL_SOURCES_LIB})
  # Link target (the lib) with libraries
  target_link_libraries(libszthpool
    ${PTHREADS_LIB}
  )
  # Set the output directories for the libraries and binaries
  set_target_properties (libszthpool
    PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  # Set compile options if on linux
  set_property(TARGET libszthpool PROPERTY COMPILE_OPTIONS ${COMPILE_OPTS})
  set_property(TARGET libszthpool PROPERTY INTERFACE_COMPILE_OPTIONS ${COMPILE_OPTS})
endif()



set(CONFIGURED_ONCE TRUE CACHE INTERNAL
    "A flag showing that CMake has configured at least once.")

