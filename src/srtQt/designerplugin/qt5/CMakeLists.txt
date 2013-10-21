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


# Find Qt5
FIND_PACKAGE(Qt5Designer 5.0.0)

IF (${Qt5Core_FOUND})

  #
  # Qt Designer plugin for Qt5
  #
  QT5_ADD_RESOURCES(srtQt5SceneWidgetPlugin_RESOURCES_RCC 
    ../SceneWidgetPlugin.qrc)
  ADD_LIBRARY(srtQt5SceneWidgetPlugin
    SHARED
    ../SceneWidgetPlugin.cpp
    ${srtQt5SceneWidgetPlugin_RESOURCES_RCC}
    )
  QT5_USE_MODULES(srtQt5SceneWidgetPlugin Core Designer Gui)
  TARGET_LINK_LIBRARIES(srtQt5SceneWidgetPlugin srtQt5Widgets)
  SET_TARGET_PROPERTIES(srtQt5SceneWidgetPlugin PROPERTIES AUTOMOC TRUE)
  MESSAGE(STATUS ${QT_PLUGINS_DIR})
  INSTALL(TARGETS srtQt5SceneWidgetPlugin DESTINATION ${QT_PLUGINS_DIR}/designer)
  
ELSE (${Qt5Core_FOUND})
  
  MESSAGE(STATUS "Qt5 development packages were not found. The Qt5 designer plugin will not be build.")
  
ENDIF (${Qt5Core_FOUND})
