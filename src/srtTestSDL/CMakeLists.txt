project(srtTestSDL)

add_definitions( -D_GNU_SOURCE -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS -DUSE_LLVM )
SET( CMAKE_C_FLAGS  "-std=c99 -march=native" )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../libsrt ${CMAKE_CURRENT_BINARY_DIR}/../libsrt ${SDL_INCLUDE_DIR} )
add_executable( srtTestSDL
  edit.c
  srt.c
  ppm.c
  )
target_link_libraries( srtTestSDL srt ${SDL_LDFLAGS} ${SDL_LIBRARY})
