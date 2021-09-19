cmake_minimum_required(VERSION 3.2)

set(LUA_MODULE_NAME "spn")

project(lua-module CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

include(FindPkgConfig)
pkg_check_modules(LIBLUA REQUIRED lua-5.3)
include_directories(${LIBLUA_INCLUDE_DIRS})


#
# module
#
set(SOURCE_FILES
  SPNTouchPool.cpp
  SPNOutput.cpp
  spn_touch.cpp
  lua_common.cpp
  lua_spn.cpp
)

add_library(lua-module MODULE ${SOURCE_FILES})
set_target_properties(lua-module PROPERTIES
  PREFIX ""     # strip `lib` prefix
  OUTPUT_NAME "${LUA_MODULE_NAME}"
)
target_link_libraries(lua-module soundplaneclient)

set(NORNS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/dep/norns")
target_include_directories(lua-module PUBLIC "${NORNS_SOURCE_DIR}/matron/src")
