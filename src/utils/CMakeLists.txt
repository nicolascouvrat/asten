set(SOURCES
  btnstream.cpp
  byte_aggregator.cpp
  logger.cpp
  screenstream.cpp)
    
add_library(utils ${SOURCES})
target_include_directories(utils PRIVATE ${INCLUDE_DIR})
