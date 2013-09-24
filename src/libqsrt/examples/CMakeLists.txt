#
# Find Qt4
#
FIND_PACKAGE(Qt4 4.8.0 REQUIRED)


#
# Minimalistic command line application
#
ADD_EXECUTABLE(minimalExampleCLI 
  minimalExampleCLI.cpp
  )
QT4_USE_MODULES(minimalExampleCLI Core Gui)
TARGET_LINK_LIBRARIES(minimalExampleCLI srt-qt4core)


#
# Minimalistic GUI application
#
ADD_EXECUTABLE(minimalExampleGUI
  minimalExampleGUI.cpp
  )
QT4_USE_MODULES(minimalExampleGUI Core Gui)
TARGET_LINK_LIBRARIES(minimalExampleGUI srt-qt4core srt-qt4widgets)
