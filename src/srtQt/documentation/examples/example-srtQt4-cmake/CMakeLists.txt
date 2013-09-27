#
# Set up cmake
#
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.6)


#
# Find Qt4
#
# We require Qt4.8.0 or newer.
#
FIND_PACKAGE(Qt4 4.8.0 REQUIRED)


#
# Find srtQt4
#
# Add include and library directories, so that compiler and linker can find what
# they need.
#
FIND_PACKAGE(srtQt4 REQUIRED)
INCLUDE_DIRECTORIES(${srtQt4_INCLUDE_DIRS})
LINK_DIRECTORIES(${srtQt4_LINK_DIRS})


#
# Minimalistic command line application
#
# Note Qt4's Gui module is required, even for applications that have no
# graphical user interface. The reason is that srtQt4 renders scenes to QImages,
# which are contained in Qt's Gui module.
#
# The command 'TARGET_LINK_LIBRARIES' pulls in the 'srtQt4Core' library, which
# is used in the program.
#
ADD_EXECUTABLE(minimalExampleCLI minimalExampleCLI.cpp)
SET_TARGET_PROPERTIES(minimalExampleCLI PROPERTIES AUTOMOC TRUE)
QT4_USE_MODULES(minimalExampleCLI Core Gui)
TARGET_LINK_LIBRARIES(minimalExampleCLI ${srtQt4Core_LIBRARY})


#
# Minimalistic GUI application
#
# The command 'TARGET_LINK_LIBRARIES' pull in the 'srtQt4Core' and
# 'srtQt4Widget' libraries, which are used in the program.
#
ADD_EXECUTABLE(minimalExampleGUI minimalExampleGUI.cpp )
SET_TARGET_PROPERTIES(minimalExampleGUI PROPERTIES AUTOMOC TRUE)
QT4_USE_MODULES(minimalExampleGUI Core Gui)
TARGET_LINK_LIBRARIES(minimalExampleGUI ${srtQt4Core_LIBRARY} ${srtQt4Widget_LIBRARY})
