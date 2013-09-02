project(qsrt)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../libsrt ${CMAKE_CURRENT_BINARY_DIR}/../libsrt ${SDL_INCLUDE_DIR} )


# libqsrt
add_library(qsrt SHARED
  srtCamera.cpp
  srtScene.cpp
  srtSurface.cpp
)
target_link_libraries( qsrt srt )
set_target_properties(qsrt PROPERTIES AUTOMOC TRUE)


# Unit tests
ADD_EXECUTABLE( qsrt_tests
  qsrt_tests.cpp
  srt_test.cpp
  srtCamera_test.cpp
  srtSurface_test.cpp
  )
TARGET_LINK_LIBRARIES( qsrt_tests qsrt ${QT_QTTEST_LIBRARY} ${QT_LIBRARIES} )
set_target_properties(qsrt_tests PROPERTIES AUTOMOC TRUE)


# Memory test
ADD_EXECUTABLE( qsrt_memtest
  qsrt_memtest.cpp
  )
TARGET_LINK_LIBRARIES( qsrt_memtest qsrt ${QT_LIBRARIES} )
set_target_properties(qsrt_tests PROPERTIES AUTOMOC TRUE)


# API documentation
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
