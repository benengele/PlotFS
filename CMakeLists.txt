cmake_minimum_required(VERSION 3.10)
project(PlotFS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "-D_FILE_OFFSET_BITS=64")
set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})

find_package(PkgConfig REQUIRED) 

# Explicitly specify the library path and include directory for FUSE3
# findPacke did not work on Debian
find_path(FUSE3_INCLUDE_DIR fuse3/fuse.h)
find_library(FUSE3_LIBRARY fuse3)

if (FUSE3_INCLUDE_DIR AND FUSE3_LIBRARY)
    message(STATUS "FUSE3 library found: ${FUSE3_LIBRARY}")
    message(STATUS "FUSE3 include directory found: ${FUSE3_INCLUDE_DIR}")
    include_directories(${FUSE3_INCLUDE_DIR})
else ()
    message(FATAL_ERROR "FUSE3 library not found. Please install libfuse3-dev or specify the path.")
endif ()

message(STATUS "FUSE3=${FUSE3_LIBRARY}")

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/plotfs_generated.h
  COMMAND flatc --cpp --gen-object-api -o ${CMAKE_CURRENT_BINARY_DIR}/ ${CMAKE_CURRENT_SOURCE_DIR}/plotfs.fbs
  DEPENDS  plotfs.fbs
)

add_executable(plotfs ${CMAKE_CURRENT_BINARY_DIR}/plotfs_generated.h cli.cpp)
target_include_directories(plotfs PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

add_executable(mount.plotfs ${CMAKE_CURRENT_BINARY_DIR}/plotfs_generated.h mount.cpp)
target_include_directories(mount.plotfs PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(mount.plotfs ${FUSE3_LIBRARY})

install(TARGETS plotfs mount.plotfs DESTINATION bin)

add_subdirectory(tests)
enable_testing()