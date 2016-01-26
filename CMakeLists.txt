
# Cmake entry point
cmake_minimum_required (VERSION 2.8.9)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/rpavlik-cmake-modules-1c73e35/")

project (szthreadpool)

# Setup handy variables:
set(SZTHREADPOOL_EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(SZTHREADPOOL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SZTHREADPOOL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Set platform-agnostic include dirs for the project
include_directories(
  ${SZTHREADPOOL_INCLUDE_DIR}
  ${TBB_INCLUDE_DIRS}
)

# Set compiler definitions
add_definitions(${TBB_DEFINITIONS})

# Set headers and sources for the szutl library
set(SZTHREADPOOL_HEADERS
  #${SZTHREADPOOL_INCLUDE_DIR}/szutl_binary_data_loader.h
)
set(SZTHREADPOOL_SOURCES
  ${SZTHREADPOOL_SOURCE_DIR}/main.cpp
)

# Create the szutl library
add_executable(szthpool ${SZTHREADPOOL_HEADERS} ${SZTHREADPOOL_SOURCES})

# Link target with libraries
#target_link_libraries(szthpool
#  ${TBB_LIBRARIES}
#)

# Set the output directories for the libraries and binaries
set_target_properties (szthpool
  PROPERTIES 
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CONFIGURED_ONCE TRUE CACHE INTERNAL
    "A flag showing that CMake has configured at least once.")

