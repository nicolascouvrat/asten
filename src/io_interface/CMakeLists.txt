#
# IO_Interface library
#

set(SOURCES
  classic_interface.cpp
  compare_interface.cpp
  io_interface.cpp
  replay_interface.cpp
  shader_program.cpp
  spy_interface.cpp)
  
add_library(io_interface ${SOURCES})
set_property(TARGET io_interface PROPERTY CXX_STANDARD 11)

target_include_directories(io_interface PRIVATE ${INCLUDE_DIR})
target_include_directories(io_interface PRIVATE ${GLFW_INCLUDE_DIR})
target_include_directories(io_interface PRIVATE ${GLAD_INCLUDE_DIR})
target_include_directories(io_interface PRIVATE ${SHADERS_DIR})

target_link_libraries(io_interface glfw)
target_link_libraries(io_interface glad)
target_link_libraries(io_interface utils)
