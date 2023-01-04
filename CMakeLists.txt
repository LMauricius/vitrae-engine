cmake_minimum_required(VERSION 3.0.0)
project(VitraeEngine VERSION 0.1.0)

include(CTest)
enable_testing()

add_subdirectory(VitraeEngine)
add_subdirectory(Showcase)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
