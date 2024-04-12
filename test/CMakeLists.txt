cmake_minimum_required(VERSION 3.10)

project(YoTest)
include(FetchContent)
enable_testing()
FetchContent_Declare(gtest
    QUIET
    URL https://codeload.github.com/google/googletest/tar.gz/refs/tags/v1.14.0
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(gtest)
set(CMAKE_CXX_STANDARD 20)
add_subdirectory(.. UniLib)
add_executable(unit_tests test_file.cpp)
target_link_libraries(unit_tests PRIVATE gtest_main misc_utils)
add_custom_command(TARGET unit_tests PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_SOURCE_DIR}/template ${CMAKE_BINARY_DIR}/template)

include(GoogleTest)
gtest_discover_tests(unit_tests PROPERTIES LABELS "File" DISCOVERY_TIMEOUT 240)

target_compile_options(unit_tests PUBLIC
    "$<$<C_COMPILER_ID:MSVC>:/utf-8>"
    "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
    "$<$<C_COMPILER_ID:MSVC>:/W4>"
    "$<$<CXX_COMPILER_ID:MSVC>:/W4>"
)
