#
# Build all tests
#
set(integration_tests
  ram_after_reset)

# This assumes several things as far as naming and folder organization are concerned. Given a
# directory "cpu"
#   - the name of the .cpp should be {dir}.cpp, so here cpu.cpp
#   - it will create a test test_{dir}, here test_cpu
#   - it will copy buttons.log and screen.log
#   - it will copy the {dir}.nes file, here cpu.nes
foreach(test ${integration_tests})
  add_executable(${test} "${test}/${test}.cpp")
  set_property(TARGET ${test} PROPERTY CXX_STANDARD 11)

  target_include_directories(${test} PRIVATE ${INCLUDE_DIR})

  target_link_libraries(${test} io_interface)
  target_link_libraries(${test} console)

  add_test("test_${test}" ${test})

  set(to_copy
    "${CMAKE_CURRENT_SOURCE_DIR}/${test}/buttons.log"
    "${CMAKE_CURRENT_SOURCE_DIR}/${test}/screen.log"
    "${CMAKE_CURRENT_SOURCE_DIR}/${test}/${test}.nes"
  )
  add_custom_command(
    TARGET ${test} POST_BUILD # do it only if build succeeds
    COMMAND ${CMAKE_COMMAND} -E copy
      ${to_copy} ${CMAKE_CURRENT_BINARY_DIR}
  )
endforeach()
