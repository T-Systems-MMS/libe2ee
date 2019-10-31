cmake_minimum_required(VERSION 3.14)
project(libe2ee)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/src/cmake")
find_package(GMP REQUIRED)
find_package(SWIG REQUIRED)
find_package(JNI REQUIRED)
include(${SWIG_USE_FILE})

add_compile_options("-fsanitize=address")
add_link_options("-fsanitize=address")

file(GLOB_RECURSE E2EE_SRCS "${PROJECT_SOURCE_DIR}/src/e2ee/*.cpp" "${PROJECT_SOURCE_DIR}/src/e2ee/*.c")

set (CMAKE_CXX_STANDARD 17)

include_directories("${GMP_INCLUDES}")
include_directories("${PROJECT_SOURCE_DIR}/src/lib/rapidjson/include")
include_directories("${PROJECT_SOURCE_DIR}/src/lib/aixlog/include")
include_directories(${JNI_INCLUDE_DIRS})
add_subdirectory("${PROJECT_SOURCE_DIR}/src/lib/pbc")

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src/lib/pbc/include")


add_library(e2ee SHARED ${E2EE_SRCS})
target_link_libraries(e2ee pbc ${GMP_LIBRARIES})

add_subdirectory(tests)
add_subdirectory(mwe)

set_property(SOURCE "${PROJECT_SOURCE_DIR}/src/je2ee.i" PROPERTY CPLUSPLUS ON)
swig_add_library(je2ee
        LANGUAGE java
        OUTPUT_DIR "${PROJECT_SOURCE_DIR}/src/java"
        SOURCES ${E2EE_SRCS} "${PROJECT_SOURCE_DIR}/src/je2ee.i")
target_link_libraries(je2ee pbc ${GMP_LIBRARIES})