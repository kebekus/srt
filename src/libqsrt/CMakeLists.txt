project(qsrt)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../libsrt ${CMAKE_CURRENT_BINARY_DIR}/../libsrt ${SDL_INCLUDE_DIR} )

add_library(qsrt SHARED
  srtCamera.cpp
  srtScene.cpp
  srtSurface.cpp
)
target_link_libraries( qsrt srt )

# API documentation
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
