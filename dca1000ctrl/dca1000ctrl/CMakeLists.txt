cmake_minimum_required(VERSION 3.0.2)
project(dca1000ctrl)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  mavros_msgs
  geometry_msgs
  message_generation
)

## Generate messages in the 'msg' folder
add_message_files(
  FILES
  instruction.msg
)

## Generate added messages and services with any dependencies listed here
generate_messages(
  DEPENDENCIES
  std_msgs  # Or other packages containing msgs
)

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES dca1000ctrl
  CATKIN_DEPENDS roscpp rospy geometry_msgs std_msgs mavros_msgs
  # DEPENDS system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
# include
  ${catkin_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/include
  ${EIGEN3_INCLUDE_DIR}
)

aux_source_directory(./include/ src_path)

## Declare a C++ library
add_library(${PROJECT_NAME} SHARED
include/linked_list.h
include/linked_list.cpp
include/radar_define.h
include/udp_define.h
include/udp_client.h
include/udp_client.cpp
include/udp_server.h
include/udp_server.cpp
include/radar_define.h
include/radar_define.cpp
include/drone_attitude.h
include/drone_attitude.cpp
)

# libserial packages
find_package(PkgConfig)
pkg_check_modules(SERIAL libserial)

## Declare a C++ executable
## With catkin_make all packages are built within a single CMake context
## The recommended prefix ensures that target names across packages don't collide
add_executable(
  main_program
  src/main_program.cpp
  ${src_path}
)

## Specify libraries to link a library or executable target against
target_link_libraries(
  main_program
  ${catkin_LIBRARIES}
  ${SERIAL_LDFLAGS} 
  ${CMAKE_THREAD_LIBS_INIT}
)

## add dependencies
add_dependencies(
  main_program
  ${PROJECT_NAME}_generate_messages_cpp
)

