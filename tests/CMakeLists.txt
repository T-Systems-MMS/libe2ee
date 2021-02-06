cmake_minimum_required(VERSION 3.11)
project(e2ee_test)
find_package(Catch2 REQUIRED)

add_compile_options("-fsanitize=address")
add_link_options("-fsanitize=address")

file(GLOB_RECURSE SRCS "${PROJECT_SOURCE_DIR}/*.cpp")

set (CMAKE_CXX_STANDARD 17)
add_executable(e2ee_test ${SRCS})
include_directories(${catch2_SOURCE_DIR}/include)
target_link_libraries(e2ee_test PRIVATE Catch2::Catch2 e2ee)

include(CTest)
include(Catch)

enable_testing()

catch_discover_tests(e2ee_test)
