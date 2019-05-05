cmake_minimum_required(VERSION 3.13.3)

### RULES FOR MAIN PROJECT ###

project(nes-cpp)
# set aliases
set(EXT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
# add sources
file(GLOB SOURCES
  "${SRC_DIR}/*.cpp"
  "${INCLUDE_DIR}/*.h"
)
# make target
add_executable(${PROJECT_NAME} ${SOURCES})
# force usage of C++ 11
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
# include directory
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
# copy shaders, this is necessary for the compiled binary to find them when running
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD # do it only if build succeeds
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SRC_DIR}/shaders ${CMAKE_CURRENT_BINARY_DIR}/shaders
)

### RULES FOR EXTERNAL LIBRAIRIES ###

# GLFW
set(GLFW_DIR "${EXT_DIR}/glfw")
# disable unneeded stuff
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "GLFW docs")
set(GLFW_INSTALL OFF CACHE INTERNAL "GLFW install target")
# use the sub CMakeLists
add_subdirectory("${GLFW_DIR}")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# GLAD
set(GLAD_DIR "${EXT_DIR}/glad")
# create our own mini library named "glad"
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
# include and link it
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")



