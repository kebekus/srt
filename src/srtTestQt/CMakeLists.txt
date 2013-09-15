project(srtTestQt)

SET(srtTestQt_SOURCES 
  exportDialog.cpp
  main.cpp
  mainWindow.cpp)

SET(srtTestQt_FORMS 
  exportDialog.ui
  mainWindow.ui)

SET(srtTestQt_RESOURCES
  srtTestQt.qrc)

QT4_ADD_RESOURCES(srtTestQt_RESOURCES_RCC ${srtTestQt_RESOURCES})
QT4_WRAP_UI(srtTestQt_FORMS_HEADERS ${srtTestQt_FORMS})

ADD_EXECUTABLE(srtTestQt
    ${srtTestQt_SOURCES} 
    ${srtTestQt_FORMS_HEADERS}
    ${srtTestQt_RESOURCES_RCC})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../libqsrt ${CMAKE_CURRENT_SOURCE_DIR}/../srtTestQt ${CMAKE_CURRENT_BINARY_DIR})

TARGET_LINK_LIBRARIES(srtTestQt qsrt ${QT_LIBRARIES})

SET_TARGET_PROPERTIES(srtTestQt PROPERTIES AUTOMOC TRUE)
