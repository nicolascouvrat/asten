cmake_minimum_required(VERSION 3.13.3)

project(lib_engine)

file(GLOB LIB_ENGINE_SRC "*.cpp")
add_library(${PROJECT_NAME} ${LIB_ENGINE_SRC})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

