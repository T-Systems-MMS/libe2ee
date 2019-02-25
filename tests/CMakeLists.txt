cmake_minimum_required(VERSION 3.10)
project(e2ee_test)

enable_testing()

file(GLOB_RECURSE SRCS "${PROJECT_SOURCE_DIR}/*.cpp")

set (CMAKE_CXX_STANDARD 17)
add_executable(e2ee_test ${SRCS})
include_directories("${PROJECT_SOURCE_DIR}/../src/lib/googletest/googletest/include")
target_link_libraries(e2ee_test e2ee gtest)


add_test(AllTestsInFoo e2ee_test)
