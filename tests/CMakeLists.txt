cmake_minimum_required(VERSION 3.10)
project(e2ee_test)

add_compile_options("-fsanitize=address")
add_link_options("-fsanitize=address")

enable_testing()

file(GLOB_RECURSE SRCS "${PROJECT_SOURCE_DIR}/*.cpp")

set (CMAKE_CXX_STANDARD 17)
add_executable(e2ee_test ${SRCS})
include_directories("${PROJECT_SOURCE_DIR}/../src/lib/catch2")
target_link_libraries(e2ee_test e2ee)

add_test(AllTestsInFoo e2ee_test)
