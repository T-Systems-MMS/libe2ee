cmake_minimum_required(VERSION 3.11)
project(e2ee_test)

include("catch2/Catch.cmake")

add_compile_options("-fsanitize=address")
add_link_options("-fsanitize=address")

file(GLOB_RECURSE SRCS "${PROJECT_SOURCE_DIR}/*.cpp")

set (CMAKE_CXX_STANDARD 17)
add_executable(e2ee_test ${SRCS})
include_directories("${PROJECT_SOURCE_DIR}/catch2")
target_link_libraries(e2ee_test e2ee)

include(CTest)

enable_testing()

catch_discover_tests(e2ee_test)
