cmake_minimum_required(VERSION 3.2)

set(LUA_MODULE_NAME "_spn")

project(lua-extension CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

include(FindPkgConfig)
pkg_check_modules(LIBLUA REQUIRED lua-5.3)
include_directories(${LIBLUA_INCLUDE_DIRS})


#
# module
#
set(SOURCE_FILES
  lua_common.cpp
  SPNControlPool.cpp
  SPNTouchPool.cpp
  SPNOutput.cpp
  spn_touch.cpp
  spn_zone.cpp
  spn_module.cpp
)

add_library(lua-extension MODULE ${SOURCE_FILES})
set_target_properties(lua-extension PROPERTIES
  PREFIX ""     # strip `lib` prefix
  OUTPUT_NAME "${LUA_MODULE_NAME}"
)
target_link_libraries(lua-extension soundplaneclient)

set(NORNS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/dep/norns")
target_include_directories(lua-extension PUBLIC "${NORNS_SOURCE_DIR}/matron/src")
