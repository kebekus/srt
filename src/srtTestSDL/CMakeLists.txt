cmake_minimum_required(VERSION 2.8.6)

project(srtTestSDL)

set(CMAKE_BUILD_TYPE Debug)

find_package(SDL)

IF (${SDL_FOUND})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libsrt ${CMAKE_CURRENT_BINARY_DIR}/../libsrt ${SDL_INCLUDE_DIR})

add_executable(srtTestSDL
	edit.cpp
	srt.cpp
	ppm.cpp
)

target_link_libraries(srtTestSDL -L../libsrt srt ${SDL_LDFLAGS} ${SDL_LIBRARY})

ELSE (${SDL_FOUND})
  
  MESSAGE(STATUS "The SDL library was not found. The application srtTestSDL will not be build.")
  
ENDIF (${SDL_FOUND})

