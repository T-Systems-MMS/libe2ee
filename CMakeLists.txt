cmake_minimum_required(VERSION 3.7)
project(libe2ee)

file(GLOB_RECURSE E2EE_SRCS "${PROJECT_SOURCE_DIR}/src/e2ee/*.cpp" "${PROJECT_SOURCE_DIR}/src/e2ee/*.c")

set (CMAKE_CXX_STANDARD 17)

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src/lib/pbc/include")
add_library(e2ee ${E2EE_SRCS})
