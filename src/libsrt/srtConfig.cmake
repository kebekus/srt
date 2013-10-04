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
# Config file for the srt package
#

# Get directory where this file is installed, compute some other directories
GET_FILENAME_COMPONENT(srt_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
GET_FILENAME_COMPONENT(srt_LNK "${srt_CMAKE_DIR}/.." ABSOLUTE)

# Set variables
SET(srt_FOUND true CACHE BOOL "Indicates that srt has been found.")
SET(srt_LINK_DIRS "${srt_LNK}" CACHE PATH "Path for srt libraries.")
SET(srt_Library srt CACHE STRING "Name of the srt core library.")
