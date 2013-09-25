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
# Setup cmake
#
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)
PROJECT(srtQt)

#
# Check compiler version
#
# If compiler is g++, require at least version 4.8.0. Older versions have
# problems with sse2 extensions
#
IF (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  IF (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.8.0")
    MESSAGE (FATAL_ERROR "Unsuitable compiler\n"
      "To compile this project with GNU compilers, use gcc version 4.8.0 or newer. "
      "Older GNU compilers have known problems with sse2 extensions, and fail to compile srtQt properly.")
  ENDIF()
ENDIF()


#
# Set version strings
#
SET(srtQt_VERSION_MAJOR 0)
SET(srtQt_VERSION_MINOR 1)
SET(srtQt_VERSION_PATCH 1)
SET(srtQt_VERSION_STRING ${srtQt_VERSION_MAJOR}.${srtQt_VERSION_MINOR}.${srtQt_VERSION_PATCH})


#
# Set include and library directories for everything that builds in this
# directory and in any of its subdirectories
#
INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}/../libsrt
  ${CMAKE_CURRENT_BINARY_DIR}/../libsrt
  ${CMAKE_CURRENT_SOURCE_DIR}
  )


#
# Add subdirectories
#
ADD_SUBDIRECTORY(designerplugin)
ADD_SUBDIRECTORY(documentation)
ADD_SUBDIRECTORY(examples)
ADD_SUBDIRECTORY(srtQt)
ADD_SUBDIRECTORY(tests)
