cmake_minimum_required(VERSION 3.10)
project(mwe)

add_compile_options("-fsanitize=address")
add_link_options("-fsanitize=address")

enable_testing()

file(GLOB_RECURSE SRCS "${PROJECT_SOURCE_DIR}/*.cpp")

set (CMAKE_CXX_STANDARD 17)
add_executable(mwe ${SRCS})
target_link_libraries(mwe pbc)

