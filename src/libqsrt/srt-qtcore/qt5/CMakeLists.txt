# Find Qt5
FIND_PACKAGE(Qt5Core 5.1.0)


IF (${Qt5Core_FOUND})

  #
  # libsrt-qt5
  #
  ADD_LIBRARY(srt-qt5core
    SHARED
    ${srt-qtcore_SOURCES}
    )
  QT5_USE_MODULES(srt-qt5core Concurrent Core Gui)
  TARGET_LINK_LIBRARIES(srt-qt5core srt)
  SET_TARGET_PROPERTIES(srt-qt5core PROPERTIES AUTOMOC TRUE)
  SET_TARGET_PROPERTIES(srt-qt5core PROPERTIES VERSION ${srt-qt_VERSION_STRING} SOVERSION ${srt-qt_VERSION_MAJOR})
  INSTALL(TARGETS srt-qt5core DESTINATION lib)
  INSTALL(FILES ${srt-qtcore_HEADERS} DESTINATION include/srt-qt5)

ELSE (${Qt5Core_FOUND})
  
  MESSAGE(STATUS "Qt5 development packages were not found. libsrt-qt5core will not be build.")
  
ENDIF (${Qt5Core_FOUND})
