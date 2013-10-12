cmake_minimum_required(VERSION 2.8.6)

#
# Enable testing
#
ENABLE_TESTING()

#
# Add uninstall target
#
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

#
# Add subdirectories
#
add_subdirectory(libsrt)
add_subdirectory(srtQt)
add_subdirectory(srtTestSDL)
add_subdirectory(srtTestQt)
