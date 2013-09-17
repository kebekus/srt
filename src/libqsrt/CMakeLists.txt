cmake_minimum_required(VERSION 2.8.6)

project(qsrt)

set(CMAKE_BUILD_TYPE Debug)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libsrt ${CMAKE_CURRENT_BINARY_DIR}/../libsrt ${SDL_INCLUDE_DIR})

find_package(Qt4 REQUIRED QtCore QtGui QtTest QtDesigner)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

find_package(Doxygen REQUIRED)


#
# libqsrt
#
add_library(qsrt SHARED
  srtCamera.cpp
  srtScene.cpp
  srtSceneWidget.cpp
  srtSurface.cpp
)
target_link_libraries(qsrt -L../libsrt srt)
set_target_properties(qsrt PROPERTIES AUTOMOC TRUE)

add_executable(qsrt_tests
	qsrt_tests.cpp
	srt_test.cpp
	srtCamera_test.cpp
	srtSurface_test.cpp
)
target_link_libraries(qsrt_tests qsrt ${QT_QTTEST_LIBRARY} ${QT_LIBRARIES})
set_target_properties(qsrt_tests PROPERTIES AUTOMOC TRUE)

add_executable(qsrt_memtest qsrt_memtest.cpp)
target_link_libraries(qsrt_memtest qsrt ${QT_LIBRARIES})
set_target_properties(qsrt_tests PROPERTIES AUTOMOC TRUE)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
	${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating API documentation with Doxygen" VERBATIM
)


add_subdirectory(QtDesignerPlugin)
