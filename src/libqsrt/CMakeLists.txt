project(qsrt)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../libsrt ${CMAKE_CURRENT_BINARY_DIR}/../libsrt ${SDL_INCLUDE_DIR} )

add_library(qsrt SHARED
  srtCamera.cpp
  srtScene.cpp
  srtSurface.cpp
)
target_link_libraries( qsrt srt )