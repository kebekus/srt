#
# Qt Designer plugin
#

# Sources
QT4_ADD_RESOURCES(scenewidgetplugin_RESOURCES_RCC srtSceneWidgetPlugin.qrc)
ADD_LIBRARY(scenewidgetplugin SHARED
  srtSceneWidgetPlugin.cpp
  ${scenewidgetplugin_RESOURCES_RCC}
)

# Compiler and Linker settings
INCLUDE_DIRECTORIES( .. )
TARGET_LINK_LIBRARIES( scenewidgetplugin qsrt )
SET_TARGET_PROPERTIES(scenewidgetplugin PROPERTIES AUTOMOC TRUE)

# Install in the Qt Plugin directory
INSTALL(TARGETS scenewidgetplugin DESTINATION ${QT_PLUGINS_DIR}/designer)
