cmake_minimum_required(VERSION 3.14)

project(ThreadSafeCacheTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wunused -pedantic-errors")

# === Options ===
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)
option(ENABLE_COVERAGE "Enable code coverage" OFF)

# === Sanity check ===
if(ENABLE_TSAN AND ENABLE_COVERAGE)
    message(FATAL_ERROR "ENABLE_TSAN and ENABLE_COVERAGE cannot be enabled at the same time.")
endif()

# === ThreadSanitizer flags ===
if(ENABLE_TSAN)
    message(STATUS "ThreadSanitizer enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -g -O1 -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=thread")
endif()

# === Coverage flags ===
if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(STATUS "Code coverage enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -g -O0")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

# === Fetch GoogleTest ===
function(add_git_submodule dir)
  find_package(Git REQUIRED)

  if(NOT EXISTS ${dir}/CMakeLists.txt)
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive -- ${dir}
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
  endif()

  add_subdirectory(${dir})
endfunction(add_git_submodule)

add_git_submodule(third_party/googletest)

enable_testing()

add_executable(run_tests tests/tests.cpp)

target_link_libraries(run_tests gtest gtest_main)

add_test(NAME RunAllTests COMMAND run_tests)

include_directories(${CMAKE_SOURCE_DIR})
