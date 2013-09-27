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
# The following variables will be defined.
#
# srtQt4_FOUND .. set to "1"
# srtQt4_INCLUDE_DIRS .. 
# srtQt4Core_LIBRARY
# srtQt4Widget_LIBRARY


# Get directory where this file is installed
GET_FILENAME_COMPONENT(srtQt4_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

# Set variables
SET(srtQt4_FOUND 1)
SET(srtQt4_INCLUDE_DIRS "${srtQt4_CMAKE_DIR}/../../include/srtQt/Qt4")
SET(srtQt4_LINK_DIRS "${srtQt4_CMAKE_DIR}/..")
SET(srtQt4Core_LIBRARY srtQt4Core)
SET(srtQt4Widget_LIBRARY srtQt4Widgets)
