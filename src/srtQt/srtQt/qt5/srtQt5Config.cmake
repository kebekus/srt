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
# Config file for the srtQt5 package
#

# Find required package 'srt'
FIND_PACKAGE(srt REQUIRED)

# Get directory where this file is installed, compute some other directories
GET_FILENAME_COMPONENT(srtQt5_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
GET_FILENAME_COMPONENT(srtQt5_INC "${srtQt5_CMAKE_DIR}/../../include/srtQt/Qt5" ABSOLUTE)
GET_FILENAME_COMPONENT(srtQt5_LNK "${srtQt5_CMAKE_DIR}/.." ABSOLUTE)

# Set variables
SET(srtQt5_FOUND true CACHE BOOL "Indicates that srtQt5 has been found.")
SET(srtQt5_INCLUDE_DIRS "${srtQt5_INC}" CACHE PATH "Include path for srtQt5 header files.")
SET(srtQt5_LINK_DIRS "${srtQt5_LNK};${srt_LINK_DIRS}" CACHE PATH "Path for srtQt5 libraries.")
SET(srtQt5_coreLibrary srtQt5Core CACHE STRING "Name of the srtQt5 core library.")
#SET(srtQt5_widgetLibrary srtQt5Widgets CACHE STRING "Name of the srtQt5 widget library.")
