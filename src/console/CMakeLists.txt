#
# Console library
#

set(SOURCES
  console.cpp
  controller.cpp
  cpu.cpp
  mapper.cpp
  memory.cpp
  ppu.cpp)

add_library(console ${SOURCES})
set_property(TARGET console PROPERTY CXX_STANDARD 11)

target_include_directories(console PRIVATE ${INCLUDE_DIR})

target_link_libraries(console io_interface)
target_link_libraries(console utils)
