#
# GLAD Library
#

# GLAD is not built using CMake, so we need to manually create a small library
# for it
set(sources glad/src/glad.c)

add_library(glad ${sources})
target_include_directories(glad PRIVATE glad/include)

#
# GLFW Library
#

# Disable unneeded stuff
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "GLFW docs")
set(GLFW_INSTALL OFF CACHE INTERNAL "GLFW install target")

# Use the sub CMakeLists
add_subdirectory(glfw)
