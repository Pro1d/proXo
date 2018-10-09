cmake_minimum_required (VERSION 2.8.11)

project (proXo)

include_directories(src)
set (CMAKE_CXX_STANDARD 14)

FILE(GLOB_RECURSE LIB_SOURCES RELATIVE "${CMAKE_SOURCE_DIR}" "src/core/*.cpp")
add_library(proxo STATIC ${LIB_SOURCES})
target_link_libraries(proxo pthread)

option(SDL_SAMPLE OFF)
option(TERM_SAMPLE ON)

add_definitions("-O3 -march=corei7 -ffast-math")

if(${SDL_SAMPLE})
  FILE(GLOB_RECURSE SAMPLE_SOURCES RELATIVE "${CMAKE_SOURCE_DIR}" "src/sample/sdl/*.cpp" "src/sample/parser/*.cpp")
  add_executable(proXo_sdl_sample ${SAMPLE_SOURCES})
  target_link_libraries(proXo_sdl_sample proxo SDL SDL_image)
  target_compile_definitions(proXo_sdl_sample PRIVATE __ENABLE_SDL__)
endif()

if(${TERM_SAMPLE})
  FILE(GLOB_RECURSE SAMPLE_SOURCES RELATIVE "${CMAKE_SOURCE_DIR}" "src/sample/term/*.cpp" "src/sample/parser/*.cpp")
  add_executable(proXo_term_sample ${SAMPLE_SOURCES})
  target_link_libraries(proXo_term_sample proxo)
endif()

