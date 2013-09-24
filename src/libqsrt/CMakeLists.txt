#
# Setup cmake
#
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)
PROJECT(srt-qt)

#
# Set version strings
#
SET(srt-qt_VERSION_MAJOR 0)
SET(srt-qt_VERSION_MINOR 1)
SET(srt-qt_VERSION_PATCH 1)
SET(srt-qt_VERSION_STRING ${srt-qt_VERSION_MAJOR}.${srt-qt_VERSION_MINOR}.${srt-qt_VERSION_PATCH})


#
# Set include and library directories for everything that builds in this
# directory and in any of its subdirectories
#
INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}/../libsrt
  ${CMAKE_CURRENT_BINARY_DIR}/../libsrt
  ${CMAKE_CURRENT_SOURCE_DIR}/srt-qtcore
  ${CMAKE_CURRENT_SOURCE_DIR}/srt-qtwidgets
  )


#
# Add subdirectories
#
ADD_SUBDIRECTORY(documentation)
ADD_SUBDIRECTORY(examples)
ADD_SUBDIRECTORY(srt-designerplugin)
ADD_SUBDIRECTORY(srt-qtcore)
ADD_SUBDIRECTORY(srt-qtwidgets)
ADD_SUBDIRECTORY(tests)
