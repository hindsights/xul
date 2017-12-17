cmake_minimum_required(VERSION 2.6)

project(xul-all)

option(xul_debug "debug-version" OFF)
option(xul_fulllog "enable all logs" OFF)

if (xul_fulllog)
  add_definitions(-DXUL_ENABLE_FULL_LOG)
endif ()

add_definitions(-D_LARGEFILE_SOURCE)
add_definitions(-D_LARGEFILE64_SOURCE)
add_definitions(-D__USE_FILE_OFFSET64)
add_definitions(-D_FILE_OFFSET_BITS=64)
add_definitions(-DBOOST_DISABLE_EXCEPTION)
add_definitions(-DBOOST_FILESYSTEM_VERSION=2)
add_definitions(-DBOOST_FILESYSTEM_NARROW_ONLY)

if (MSVC)
  include_directories("include/xul/config/msvc")
  add_definitions(-DWIN32_LEAN_AND_MEAN)
  add_definitions(-D_WIN32_WINNT=0x0501)
else ()
  add_definitions(-DINET_ADDRSTRLEN=16)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -D_DEBUG ")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -DNDEBUG ")
  if (xul_debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -D_DEBUG")
    message("enable debug build ===================")
  endif ()
  include_directories(/usr/local/include)
  link_directories(/usr/local/lib)
endif ()


include_directories("include")
add_subdirectory(examples)
