cmake_minimum_required(VERSION 3.10)
project(libe2ee)

add_compile_options("-fsanitize=address")
add_link_options("-fsanitize=address")

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/src/cmake")

file(GLOB_RECURSE E2EE_SRCS "${PROJECT_SOURCE_DIR}/src/e2ee/*.cpp" "${PROJECT_SOURCE_DIR}/src/e2ee/*.c")

set (CMAKE_CXX_STANDARD 17)

find_package(GMP REQUIRED)
include_directories("${GMP_INCLUDES}")
include_directories("${PROJECT_SOURCE_DIR}/src/lib/rapidjson/include")
include_directories("${PROJECT_SOURCE_DIR}/src/lib/aixlog/include")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/lib/pbc")

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src/lib/pbc/include")


add_library(e2ee SHARED ${E2EE_SRCS})
target_link_libraries(e2ee pbc ${GMP_LIBRARIES})

add_subdirectory(tests)