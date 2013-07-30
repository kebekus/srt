project(srtTestQt)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../libsrt ${CMAKE_CURRENT_BINARY_DIR}/../libsrt ${SDL_INCLUDE_DIR} )
add_executable( srtTestQt
  edit.cpp
  srt.cpp
  ppm.cpp
  )
target_link_libraries( srtTestQt srt ${SDL_LDFLAGS} ${SDL_LIBRARY} ${QT_LIBRARIES})
