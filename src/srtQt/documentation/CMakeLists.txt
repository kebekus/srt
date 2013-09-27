#
# Find Doxygen and configure 'Doxyfile'
#
FIND_PACKAGE(Doxygen REQUIRED)


#
# Find Qt4
#
FIND_PACKAGE(Qt4 4.8.0 REQUIRED)


#
# logoMaker
#
# This is a little program that is used to render the logo of the srtQt project.
#
ADD_EXECUTABLE(logoMaker
  logoMaker.cpp
  )
QT4_USE_MODULES(logoMaker Core Gui)
TARGET_LINK_LIBRARIES(logoMaker srtQt4Core)


#
# API documentation
#
# In order to re-build the documentation only when one of the header files
# changes, I have added a dummy executable here, which includes all the header
# files from srt-qt4core and srt-qt5core. Generating the documentation with
# doxygen is part of its build process.
#
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
ADD_EXECUTABLE(doc
  dummy.cpp)
QT4_USE_MODULES(doc Core Gui)
ADD_CUSTOM_COMMAND(TARGET doc POST_BUILD
  DEPENDS logoMaker
  COMMAND logoMaker ${CMAKE_CURRENT_SOURCE_DIR}/logo.ssc
  COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating API documentation with Doxygen"
  )
INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/apidoc/html DESTINATION share/doc/srtQt-${srtQt_VERSION_STRING}
  )


#
# Example Projects
#
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples DESTINATION share/doc/srtQt-${srtQt_VERSION_STRING}
  )
