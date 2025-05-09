cmake_minimum_required(VERSION 3.14)

project(ThreadSafeCacheTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(run_tests tests.cpp)

target_link_libraries(run_tests gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(run_tests)

include_directories(${CMAKE_SOURCE_DIR})
