project(examples)

ADD_EXECUTABLE(minimalExample
  minimalExample.cpp)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../libqsrt  ${CMAKE_CURRENT_BINARY_DIR})

TARGET_LINK_LIBRARIES(minimalExample qsrt ${QT_LIBRARIES})

