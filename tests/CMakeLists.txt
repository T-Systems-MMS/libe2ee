cmake_minimum_required(VERSION 3.11)
project(e2ee_test)
include(FetchContent)

add_compile_options("-fsanitize=address")
add_link_options("-fsanitize=address")

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v2.13.4)

FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)

file(GLOB_RECURSE SRCS "${PROJECT_SOURCE_DIR}/*.cpp")

set (CMAKE_CXX_STANDARD 17)
add_executable(e2ee_test ${SRCS})
include_directories("${PROJECT_SOURCE_DIR}/../src/lib/catch2")
target_link_libraries(e2ee_test e2ee)

include(CTest)
include(Catch)

enable_testing()

catch_discover_tests(e2ee_test)
