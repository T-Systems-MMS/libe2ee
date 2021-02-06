cmake_minimum_required(VERSION 3.14)
project(libe2ee)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/src/cmake")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# https://stackoverflow.com/questions/51047978/cmake-could-not-find-jni
# We are only interested in finding jni.h: we do not care about extended JVM
# functionality or the AWT library.
set(JAVA_AWT_LIBRARY NotNeeded)
set(JAVA_JVM_LIBRARY NotNeeded)
set(JAVA_AWT_INCLUDE_PATH NotNeeded)
find_package(GMP REQUIRED)
find_package(SWIG REQUIRED)
find_package(JNI REQUIRED)
find_package(Boost)
include(${SWIG_USE_FILE})

add_compile_options("-fsanitize=address")
add_link_options("-fsanitize=address")

set(SHA2_SRCS "${PROJECT_SOURCE_DIR}/src/lib/sha2/sha2.c")
file(GLOB_RECURSE E2EE_SRCS "${PROJECT_SOURCE_DIR}/src/e2ee/*.cpp" "${PROJECT_SOURCE_DIR}/src/e2ee/*.c" )

set (CMAKE_CXX_STANDARD 17)

include_directories("${GMP_INCLUDES}")
include_directories("${PROJECT_SOURCE_DIR}/src/lib/rapidjson/include")
include_directories("${PROJECT_SOURCE_DIR}/src/lib/aixlog/include")
include_directories("${PROJECT_SOURCE_DIR}/src/lib/sha2")
include_directories(${JNI_INCLUDE_DIRS})
add_subdirectory("${PROJECT_SOURCE_DIR}/src/lib/pbc_cmake")

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src/lib/pbc/include")
include_directories(${Boost_INCLUDE_DIRS})


add_library(e2ee SHARED ${E2EE_SRCS} ${SHA2_SRCS})
target_link_libraries(e2ee pbc ${GMP_LIBRARIES})

add_subdirectory(tests)
add_subdirectory(mwe)

set_property(SOURCE "${PROJECT_SOURCE_DIR}/src/je2ee.i" PROPERTY CPLUSPLUS ON)
set(CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS} "-package" "com.tsystems_mms.je2ee")
swig_add_library(je2ee
        LANGUAGE java
        OUTPUT_DIR "${PROJECT_SOURCE_DIR}/src/java/com/tsystems_mms/je2ee"
        SOURCES ${E2EE_SRCS} ${SHA2_SRCS} "${PROJECT_SOURCE_DIR}/src/je2ee.i")
swig_link_libraries(je2ee pbc ${GMP_LIBRARIES})