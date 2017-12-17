cmake_minimum_required(VERSION 2.6)

# add_definitions(-DXUL_ENABLE_FULL_LOG)

# to reduce compiltation time, create a standalone library for libxul.cpp
add_library(xul STATIC libxul.cpp)

if (MSVC)
  target_link_libraries(xul ws2_32.lib)
else ()
  target_link_libraries(xul dl boost_thread.a boost_system.a boost_filesystem.a pthread z)
  if (NOT APPLE)
    target_link_libraries(xul rt)
  endif ()
endif ()

add_executable(httpsvr httpsvr.cpp)
target_link_libraries(httpsvr xul)

add_executable(xlua xlua.cpp)
target_link_libraries(xlua xul lua)

add_library(luaxul SHARED luaxul.cpp)
target_link_libraries(luaxul xul lua)
