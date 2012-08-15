cmake_minimum_required(VERSION 2.8)
project(llog)

find_package(catkin)
catkin_stack()

catkin_project(llog
  INCLUDE_DIRS include
  LIBRARIES llog
)

SET(CMAKE_BUILD_TYPE Debug)

include_directories(include)

add_library(llog
  src/logger.cpp
)

add_executable(llog_test
  src/test.cpp
)

target_link_libraries(llog_test -lstdc++)