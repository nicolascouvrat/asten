cmake_minimum_required(VERSION 3.13.3)

# Force CXX 11
SET(CMAKE_CXX_STANDARD 11)
### RULES FOR MAIN PROJECT ###

project(nes-cpp)
# set aliases
set(EXT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(GLFW_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include")
set(GLAD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include")

set(ROM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/roms")
set(SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders")

# add sources
file(GLOB SOURCES "nes.cpp")
# make target
add_executable(${PROJECT_NAME} ${SOURCES})
# force usage of C++ 11
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
# include directory
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
# copy game
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD # do it only if build succeeds
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${ROM_DIR} ${CMAKE_CURRENT_BINARY_DIR}/roms
)

### RULES FOR EXTERNAL LIBRARIES ###

add_subdirectory(external)

### RULES FOR INTERNAL LIBRARIES ###

# UTILS
target_link_libraries(${PROJECT_NAME} "utils")
add_subdirectory("${SRC_DIR}/utils")

# INTERFACE
add_subdirectory("${SRC_DIR}/io_interface")
target_include_directories("io_interface" PRIVATE ${INCLUDE_DIR})
# io_interface is the place where glfw and glad are actually used, so the linking needs to be done at this step
target_include_directories("io_interface" PRIVATE ${GLFW_INCLUDE_DIR})
target_include_directories("io_interface" PRIVATE ${GLAD_INCLUDE_DIR})
target_include_directories("io_interface" PRIVATE ${SHADERS_DIR})
target_link_libraries("io_interface" "glfw" "${GLFW_LIBRARIES}")
target_link_libraries("io_interface" "glad" "${CMAKE_DL_LIBS}")
target_link_libraries("io_interface" "utils")
target_link_libraries(${PROJECT_NAME} "io_interface")

# CONSOLE
add_subdirectory("${SRC_DIR}/console")
target_link_libraries(${PROJECT_NAME} "console")

# TESTING
SET(BUILD_TESTING
  "ON"
  CACHE
  BOOL
  "Build with tests"
)

IF (BUILD_TESTING)
  ENABLE_TESTING()
  ADD_SUBDIRECTORY("tests")
ENDIF() 
