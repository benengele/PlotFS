cmake_minimum_required(VERSION 3.10)
project(PlotFSTest)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/58d77fa8070e8cec2dc1ed015d66b454c8d78850.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  plotfs_test
  plotfs_test.cpp
  devicefactory_test.cpp
  device_test.cpp
)

# Add the include directories
target_include_directories(plotfs_test PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/../)
target_link_libraries(plotfs_test gtest_main gmock_main pthread)

include(GoogleTest)
gtest_discover_tests(plotfs_test)