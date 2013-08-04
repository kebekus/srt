project(srtTestQt)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../libqsrt ${CMAKE_CURRENT_SOURCE_DIR}/../libsrt )

add_executable( srtTestQt main.cpp )

target_link_libraries( srtTestQt qsrt ${QT_LIBRARIES})
