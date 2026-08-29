cmake_minimum_required(VERSION 3.16)
project(wlrootspp VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(WLROOTS REQUIRED IMPORTED_TARGET wlroots-0.20)
pkg_check_modules(SCENEFX REQUIRED IMPORTED_TARGET scenefx-0.5)
pkg_check_modules(WAYLAND_SERVER REQUIRED IMPORTED_TARGET wayland-server)
pkg_check_modules(XKBCOMMON REQUIRED IMPORTED_TARGET xkbcommon)
pkg_check_modules(PIXMAN REQUIRED IMPORTED_TARGET pixman-1)

file(GLOB_RECURSE WLROOTSPP_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE WLROOTSPP_HEADERS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

add_library(wlrootspp ${WLROOTSPP_SOURCES} ${WLROOTSPP_HEADERS})
target_include_directories(wlrootspp PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_definitions(wlrootspp PUBLIC WLR_USE_UNSTABLE)
target_link_libraries(wlrootspp PUBLIC
  PkgConfig::SCENEFX
  PkgConfig::WLROOTS
  PkgConfig::WAYLAND_SERVER
  PkgConfig::XKBCOMMON
  PkgConfig::PIXMAN)
set_target_properties(wlrootspp PROPERTIES
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN YES)

add_subdirectory(examples)
