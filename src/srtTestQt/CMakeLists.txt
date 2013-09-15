cmake_minimum_required(VERSION 2.8.6)

project(srtTestQt)

set(CMAKE_BUILD_TYPE Debug)

find_package(Qt4 REQUIRED QtCore QtGui QtTest)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

set(srtTestQt_SOURCES
	exportDialog.cpp
	main.cpp
	mainWindow.cpp
	srtWidget.cpp
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

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libqsrt ${CMAKE_CURRENT_SOURCE_DIR}/../srtTestQt ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(srtTestQt -L../libqsrt qsrt -L../libsrt srt ${QT_LIBRARIES})

set_target_properties(srtTestQt PROPERTIES AUTOMOC TRUE)

