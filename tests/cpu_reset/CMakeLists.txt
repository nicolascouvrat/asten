ADD_EXECUTABLE("test_ram_after_reset" "test_ram_after_reset.cpp")
set_property(TARGET test_ram_after_reset PROPERTY CXX_STANDARD 11)

ADD_TEST("ram_after_reset" "test_ram_after_reset")
TARGET_INCLUDE_DIRECTORIES("test_ram_after_reset" PRIVATE ${INCLUDE_DIR})
TARGET_LINK_LIBRARIES("test_ram_after_reset" "io_interface")
TARGET_LINK_LIBRARIES("test_ram_after_reset" "console")
add_custom_command(
  TARGET "test_ram_after_reset" POST_BUILD # do it only if build succeeds
  COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/buttons.log" "${CMAKE_CURRENT_SOURCE_DIR}/screen.log" "${CMAKE_CURRENT_SOURCE_DIR}/ram_after_reset.nes"
    ${CMAKE_CURRENT_BINARY_DIR}
)
