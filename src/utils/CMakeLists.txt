cmake_minimum_required(VERSION 3.13.3)

project(lib_utils)

file(GLOB LIB_UTILS_SRC "*.cpp")
add_library(${PROJECT_NAME} ${LIB_UTILS_SRC})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
