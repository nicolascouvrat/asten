#
# Utils library
#

set(SOURCES
  btnstream.cpp
  byte_aggregator.cpp
  logger.cpp
  screenstream.cpp)
    
add_library(utils ${SOURCES})
set_property(TARGET utils PROPERTY CXX_STANDARD 11)

target_include_directories(utils PRIVATE ${INCLUDE_DIR})
