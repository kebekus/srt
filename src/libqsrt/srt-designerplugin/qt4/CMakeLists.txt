# Find Qt4
FIND_PACKAGE(Qt4 4.8.0 REQUIRED)


#
# Qt Designer plugin for Qt4
#
QT4_ADD_RESOURCES(scenewidgetplugin-qt4_RESOURCES_RCC 
  ../srtSceneWidgetPlugin.qrc)
ADD_LIBRARY(scenewidgetplugin-qt4
  SHARED
  ../srtSceneWidgetPlugin.cpp
  ${scenewidgetplugin-qt4_RESOURCES_RCC}
)
QT4_USE_MODULES(scenewidgetplugin-qt4 Core Designer Gui)
TARGET_LINK_LIBRARIES(scenewidgetplugin-qt4 srt-qt4core)
SET_TARGET_PROPERTIES(scenewidgetplugin-qt4 PROPERTIES AUTOMOC TRUE)
INSTALL(TARGETS scenewidgetplugin-qt4 DESTINATION ${QT_PLUGINS_DIR}/designer)
