cmake_minimum_required(VERSION 3.13.3)

project(console)

file(GLOB LIB_CONSOLE_SRC "*.cpp")
add_library(${PROJECT_NAME} ${LIB_CONSOLE_SRC})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

