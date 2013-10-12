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
# Find Qt4
#
FIND_PACKAGE(Qt4 4.8.0 REQUIRED)

#
# Unit Tests
#
ADD_EXECUTABLE(srtQt4Tests
  srtQtTests.cpp
  test_Camera.cpp
  test_srt.cpp
  test_Surface.cpp
)
QT4_USE_MODULES(srtQt4Tests Core Gui Test)
TARGET_LINK_LIBRARIES(srtQt4Tests srtQt4Core)
SET_TARGET_PROPERTIES(srtQt4Tests PROPERTIES AUTOMOC TRUE)
ADD_TEST(NAME srtQt4Tests
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/srtQt4Tests)


#
# Memory Test
#
ADD_EXECUTABLE(srtQt4Memtest
  srtQtMemtest.cpp)
QT4_USE_MODULES(srtQt4Memtest Core)
TARGET_LINK_LIBRARIES(srtQt4Memtest srtQt4Core)
SET_TARGET_PROPERTIES(srtQt4Memtest PROPERTIES AUTOMOC TRUE)
ADD_TEST(NAME srtQt4Memtest
  COMMAND valgrind --error-exitcode=1 ${CMAKE_CURRENT_BINARY_DIR}/srtQt4Memtest)
