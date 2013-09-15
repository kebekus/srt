project(examples)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../libqsrt  ${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(minimalExampleCLI
  minimalExampleCLI.cpp)
TARGET_LINK_LIBRARIES(minimalExampleCLI qsrt ${QT_LIBRARIES})

ADD_EXECUTABLE(minimalExampleGUI
  minimalExampleGUI.cpp)
TARGET_LINK_LIBRARIES(minimalExampleGUI qsrt ${QT_LIBRARIES})
