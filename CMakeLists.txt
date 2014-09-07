cmake_minimum_required(VERSION 2.8.4)
project(dps)
add_definitions(--std=c++1y)

include(cmake/dev_root.cmake)
include(cmake/target_os.cmake)
include(cmake/lib_type.cmake)

include_directories(include)
include_directories(private_include)

file(GLOB PUB_INCLUDES include/*)
file(GLOB PRIV_INCLUDES include/*)
file(GLOB SOURCES src/*)

add_executable(FaceCompare ${SOURCES} ${PUB_INCLUDES} ${PRIV_INCLUDES})