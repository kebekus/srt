cmake_minimum_required(VERSION 2.8.6)

project(examples)

set(CMAKE_BUILD_TYPE Debug)

find_package(Qt4 REQUIRED QtCore QtGui QtTest)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_executable(minimalExampleCLI minimalExampleCLI.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libqsrt  ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(minimalExampleCLI -L../libqsrt qsrt -L../libsrt srt ${QT_LIBRARIES})


ADD_EXECUTABLE(minimalExampleGUI
  minimalExampleGUI.cpp)
TARGET_LINK_LIBRARIES(minimalExampleGUI qsrt ${QT_LIBRARIES})
