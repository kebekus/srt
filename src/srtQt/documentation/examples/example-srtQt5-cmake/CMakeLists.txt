#
# Set up cmake
#
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)


#
# Find Qt5
#
# We require Qt5.0.0 or newer.
#
FIND_PACKAGE(Qt5Core 5.0.0 REQUIRED)


#
# Find srtQt5
#
# Add include and library directories, so that compiler and linker can find what
# they need.
#
FIND_PACKAGE(srtQt5 REQUIRED)
INCLUDE_DIRECTORIES(${srtQt5_INCLUDE_DIRS})
LINK_DIRECTORIES(${srtQt5_LINK_DIRS})


#
# Minimalistic command line application
#
# Note Qt5's Gui module is required, even for applications that have no
# graphical user interface. The reason is that srtQt5 renders scenes to QImages,
# which are contained in Qt's Gui module.
#
# The command 'TARGET_LINK_LIBRARIES' pulls in the 'srtQt5Core' library, which
# is used in the program.
#
ADD_EXECUTABLE(minimalExampleCLI minimalExampleCLI.cpp)
SET_TARGET_PROPERTIES(minimalExampleCLI PROPERTIES AUTOMOC TRUE)
QT5_USE_MODULES(minimalExampleCLI Core Gui)
TARGET_LINK_LIBRARIES(minimalExampleCLI ${srtQt5_coreLibrary})
