#
# Find Qt4
#
FIND_PACKAGE(Qt4 4.8.0 REQUIRED)


#
# Unit Tests
#
ADD_EXECUTABLE(srt-qt4-tests
  qsrt_tests.cpp
  srt_test.cpp
  srtCamera_test.cpp
  srtSurface_test.cpp
)
QT4_USE_MODULES(srt-qt4-tests Core Gui Test)
TARGET_LINK_LIBRARIES(srt-qt4-tests srtQt4Core)
SET_TARGET_PROPERTIES(srt-qt4-tests PROPERTIES AUTOMOC TRUE)


#
# Memory Test
#
ADD_EXECUTABLE(srt-qt4-memtest
  qsrt_memtest.cpp)
QT4_USE_MODULES(srt-qt4-memtest Core)
TARGET_LINK_LIBRARIES(srt-qt4-memtest srtQt4Core)
SET_TARGET_PROPERTIES(srt-qt4-memtest PROPERTIES AUTOMOC TRUE)
