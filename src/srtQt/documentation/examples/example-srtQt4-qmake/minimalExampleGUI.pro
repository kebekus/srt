#
# Set compiler and linker flags
#
QMAKE_CXXFLAGS += $$system(cmake --find-package -DNAME=srtQt4 -DLANGUAGE=C -DCOMPILER_ID=GNU -DMODE=COMPILE)
QMAKE_LFLAGS   += $$system(cmake --find-package -DNAME=srtQt4 -DLANGUAGE=C -DCOMPILER_ID=GNU -DMODE=LINK) 
# Add the srtQt4Core library. Since this library uses the 'srt' library internally, we must also list it here
QMAKE_LIBS     += -lsrtQt4Core -lsrt
# Since we are using widgets in the example program, add the srtQt4Widgets library
QMAKE_LIBS     += -lsrtQt4Widgets


#
# The library setQt4Widgets uses GL widgets internally. We therefore need to include Qt's openGL module here
#
QT             += opengl

# List of all source files
SOURCES        += minimalExampleGUI.cpp

# Name of the binary that will eventually be generated
TARGET          = minimalExampleGUI
