cmake_minimum_required(VERSION 3.10)

project(misc_utils)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(src CURRENT_SRC)

file(GLOB_RECURSE CURRENT_INC "include/*" "include_private/*.hpp" "include_private/*.h")


add_library(${PROJECT_NAME}
    STATIC
    ${CURRENT_SRC}
    ${CURRENT_INC}
    src/algorithm/lzw.cpp
    src/algorithm/_lzw.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    include/Yo
    include_private
    include_private/algorithm
)

target_compile_definitions(${PROJECT_NAME} PRIVATE _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)

target_compile_definitions(${PROJECT_NAME} PUBLIC WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0601)

target_compile_options(${PROJECT_NAME} PUBLIC
    "$<$<C_COMPILER_ID:MSVC>:/utf-8>"
    "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
    "$<$<C_COMPILER_ID:MSVC>:/W4>"
    "$<$<CXX_COMPILER_ID:MSVC>:/W4>"
)
