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
# Config file for the srtQt4 package
#

# Find required package 'srt'
FIND_PACKAGE(srt REQUIRED)

# Get directory where this file is installed, compute some other directories
GET_FILENAME_COMPONENT(srtQt4_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
GET_FILENAME_COMPONENT(srtQt4_INC "${srtQt4_CMAKE_DIR}/../../include/srtQt/Qt4" ABSOLUTE)
GET_FILENAME_COMPONENT(srtQt4_LNK "${srtQt4_CMAKE_DIR}/.." ABSOLUTE)

# Set variables
SET(srtQt4_FOUND true CACHE BOOL "Indicates that srtQt4 has been found.")
SET(srtQt4_INCLUDE_DIRS "${srtQt4_INC}" CACHE PATH "Include path for srtQt4 header files.")
SET(srtQt4_LINK_DIRS "${srtQt4_LNK};${srt_LINK_DIRS}" CACHE PATH "Path for srtQt4 libraries.")
SET(srtQt4_coreLibrary srtQt4Core CACHE STRING "Name of the srtQt4 core library.")
SET(srtQt4_widgetLibrary srtQt4Widgets CACHE STRING "Name of the srtQt4 widget library.")
