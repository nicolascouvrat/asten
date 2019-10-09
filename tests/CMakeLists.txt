#
# Build all tests
#
set(integration_tests
  ram_after_reset
  nestest)

# This assumes several things as far as naming and folder organization are concerned. Given a
# directory "cpu"
#   - the name of the .cpp should be {dir}.cpp, so here cpu.cpp
#   - the nes code file should be {dir}.nes, here cpu.nes
#   - the button log file should be {dir}.btn, here cpu.btn
#   - the screen log file should be {dir}.scrn, here cpu.scrn
# It will create a test test_{dir}, here test_cpu
foreach(test ${integration_tests})
  add_executable(${test} "${test}/${test}.cpp")
  set_property(TARGET ${test} PROPERTY CXX_STANDARD 11)

  target_include_directories(${test} PRIVATE ${INCLUDE_DIR})

  target_link_libraries(${test} io_interface)
  target_link_libraries(${test} console)

  add_test("test_${test}" ${test})

  set(to_copy
    "${CMAKE_CURRENT_SOURCE_DIR}/${test}/${test}.btn"
    "${CMAKE_CURRENT_SOURCE_DIR}/${test}/${test}.scrn"
    "${CMAKE_CURRENT_SOURCE_DIR}/${test}/${test}.nes"
  )
  add_custom_command(
    TARGET ${test} POST_BUILD # do it only if build succeeds
    COMMAND ${CMAKE_COMMAND} -E copy
      ${to_copy} ${CMAKE_CURRENT_BINARY_DIR}
  )
endforeach()
