cmake_minimum_required (VERSION 2.8.11)

project (proXo)

include_directories(src)

FILE(GLOB_RECURSE LIB_SOURCES RELATIVE "${CMAKE_SOURCE_DIR}" "src/core/*.cpp")
add_library(proxo STATIC ${LIB_SOURCES})
target_link_libraries(proxo pthread)

option(SDL_SAMPLE OFF)

if(${SDL_SAMPLE})
  FILE(GLOB_RECURSE SAMPLE_SOURCES RELATIVE "${CMAKE_SOURCE_DIR}" "src/sample/*.cpp")
  add_executable(proXo_sample ${SAMPLE_SOURCES})
  target_link_libraries(proXo_sample proxo SDL SDL_image)
endif()
