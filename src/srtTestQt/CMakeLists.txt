cmake_minimum_required(VERSION 2.8.6)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libqsrt/srt-qtcore
  ${CMAKE_CURRENT_SOURCE_DIR}/../libqsrt/srt-qtwidgets)

project(srtTestQt)


# Find Qt4
FIND_PACKAGE(Qt4 4.8.0 REQUIRED)

set(srtTestQt_SOURCES
	exportDialog.cpp
	main.cpp
	mainWindow.cpp
)

set(srtTestQt_FORMS
	exportDialog.ui
	mainWindow.ui
)

set(srtTestQt_RESOURCES
	srtTestQt.qrc
)

qt4_add_resources(srtTestQt_RESOURCES_RCC ${srtTestQt_RESOURCES})
qt4_wrap_ui(srtTestQt_FORMS_HEADERS ${srtTestQt_FORMS})

add_executable(srtTestQt
	${srtTestQt_SOURCES}
	${srtTestQt_FORMS_HEADERS}
	${srtTestQt_RESOURCES_RCC}
)
QT4_USE_MODULES(srtTestQt Core Gui)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libqsrt ${CMAKE_CURRENT_SOURCE_DIR}/../srtTestQt ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(srtTestQt -L../libqsrt srt-qt4core srt-qt4widgets -L../libsrt srt ${QT_LIBRARIES})

set_target_properties(srtTestQt PROPERTIES AUTOMOC TRUE)
