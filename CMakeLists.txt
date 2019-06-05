cmake_minimum_required(VERSION 2.8.3)
project(turtlebot3_example_cpp)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  actionlib
  actionlib_msgs
  geometry_msgs
  message_generation
  nav_msgs
  roscpp
  rospy
  sensor_msgs
  std_msgs
  turtlebot3_msgs
  visualization_msgs
)

add_action_files(
   FILES
   Turtlebot3.action
)

generate_messages(
  DEPENDENCIES
  std_msgs
  geometry_msgs
  actionlib_msgs
)


catkin_package(
  CATKIN_DEPENDS
    actionlib
    actionlib_msgs
    geometry_msgs
    message_runtime
    nav_msgs
    roscpp
    rospy
    sensor_msgs
    std_msgs
    turtlebot3_msgs
    visualization_msgs   
)


include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)
add_executable(turtlebot3_server src/turtlebot3_server)
target_link_libraries(turtlebot3_server
	${catkin_LIBRARIES}
)

add_executable(turtlebot3_client src/turtlebot3_client)
target_link_libraries(turtlebot3_client
	${catkin_LIBRARIES}
)

add_executable(turtlebot3_obstacle src/turtlebot3_obstacle.cpp)
target_link_libraries(turtlebot3_obstacle
    ${catkin_LIBRARIES}
)

add_executable(turtlebot3_follower src/turtlebot3_follower.cpp)
target_link_libraries(turtlebot3_follower
	${catkin_LIBRARIES}
)

install(TARGETS 
	turtlebot3_server 
	turtlebot3_client
	ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY launch rviz
	DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

