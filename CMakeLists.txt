cmake_minimum_required(VERSION 2.8)
project(hns)

find_package(catkin)
catkin_stack()

catkin_project(hns
  INCLUDE_DIRS include
)

SET(CMAKE_BUILD_TYPE Debug)

find_package(Boost COMPONENTS system thread regex signals)

include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(hsn_test
  src/test.cpp
)

target_link_libraries(hsn_test ${BOOST_LIBRARIES} -lstdc++)