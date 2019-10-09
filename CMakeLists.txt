cmake_minimum_required(VERSION 3.13.3)

project(asten)

#
# Options
#

set(BUILD_TESTING
  "ON"
  CACHE
  BOOL
  "Build with tests"
)

#
# Variables
#

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(GLFW_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include")
set(GLAD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include")
set(ROM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/roms")
set(SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders")

#
# Build libraries
#

add_subdirectory(external)
add_subdirectory("${SRC_DIR}/utils")
add_subdirectory("${SRC_DIR}/io_interface")
add_subdirectory("${SRC_DIR}/console")

#
# Main executable
#

set(SOURCES asten.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} console)
target_link_libraries(${PROJECT_NAME} utils)


# Copy rom files after build success
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${ROM_DIR} ${CMAKE_CURRENT_BINARY_DIR}/roms
)

#
# Testing
#

IF (BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
ENDIF() 

#
# Install
#

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
