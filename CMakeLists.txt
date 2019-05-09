cmake_minimum_required(VERSION 3.10)
project(libe2ee)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/src/cmake")

file(GLOB_RECURSE E2EE_SRCS "${PROJECT_SOURCE_DIR}/src/e2ee/*.cpp" "${PROJECT_SOURCE_DIR}/src/e2ee/*.c")

set (CMAKE_CXX_STANDARD 17)

include_directories("${GMP_INCLUDE_DIR}")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/lib/pbc")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/lib/json-c")

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src/lib/pbc/include")

find_package(GMP REQUIRED)

add_library(e2ee SHARED ${E2EE_SRCS})
target_link_libraries(e2ee pbc ${GMP_LIBRARIES} json-c)

add_subdirectory("${PROJECT_SOURCE_DIR}/src/lib/googletest")
set_target_properties(gtest PROPERTIES FOLDER extern)
set_target_properties(gtest_main PROPERTIES FOLDER extern)
set_target_properties(gmock PROPERTIES FOLDER extern)
set_target_properties(gmock_main PROPERTIES FOLDER extern)

add_subdirectory(tests)