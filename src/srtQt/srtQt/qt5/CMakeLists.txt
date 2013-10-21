#
#   Copyright (C) 2013 Stefan Kebekus
#   stefan.kebekus@math.uni-freiburg.de
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the
#   Free Software Foundation, Inc.,
#   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#

#
# Find Qt5
#
FIND_PACKAGE(Qt5Core 5.0.0)


IF (${Qt5Core_FOUND})

  #
  # srtQt5Core
  #
  ADD_LIBRARY(srtQt5Core
    SHARED
    ${srtQtCore_SOURCES}
    )
  QT5_USE_MODULES(srtQt5Core Concurrent Core Gui)
  TARGET_LINK_LIBRARIES(srtQt5Core srt)
  SET_TARGET_PROPERTIES(srtQt5Core PROPERTIES AUTOMOC TRUE)
  SET_TARGET_PROPERTIES(srtQt5Core PROPERTIES VERSION ${srtQt_VERSION_STRING} SOVERSION ${srtQt_VERSION_MAJOR})
  INSTALL(TARGETS srtQt5Core DESTINATION lib)
  INSTALL(FILES ${srtQtCore_HEADERS} DESTINATION include/srtQt/Qt5/srtQt)


  #
  # srtQt5Widgets
  #
  ADD_LIBRARY(srtQt5Widgets
    SHARED
    ${srtQtWidgets_SOURCES}
    )
  QT5_USE_MODULES(srtQt5Widgets Core Gui OpenGL)
  TARGET_LINK_LIBRARIES(srtQt5Widgets srtQt5Core)
  SET_TARGET_PROPERTIES(srtQt5Widgets PROPERTIES AUTOMOC TRUE)
  SET_TARGET_PROPERTIES(srtQt5Widgets PROPERTIES VERSION ${srtQt_VERSION_STRING} SOVERSION ${srtQt_VERSION_MAJOR})
  INSTALL(TARGETS srtQt5Widgets DESTINATION lib)
  INSTALL(FILES ${srtQtWidgets_HEADERS} DESTINATION include/srtQt/Qt5/srtQt)
  
  
  #
  # CMake config file
  #
  INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/srtQt5Config.cmake DESTINATION lib/srtQt5-${srtQt_VERSION_STRING})
  
ELSE (${Qt5Core_FOUND})
  
  MESSAGE(STATUS "Qt5 development packages were not found. The libraries srtQt5Core and srtQt5Widgets will not be build.")
  
ENDIF (${Qt5Core_FOUND})
