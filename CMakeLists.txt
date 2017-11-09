cmake_minimum_required(VERSION 2.8.3)
project(labelizer)

add_compile_options(-std=c++11)

set(catkin_RUN_PACKAGES
	rospy
	roscpp
	roslib
)

set(catkin_BUILD_PACKAGES
	${catkin_RUN_PACKAGES}
	cmake_modules
)

## Find catkin macros and libraries
find_package(catkin REQUIRED COMPONENTS
	${catkin_BUILD_PACKAGES}
)

## Find several needed libraries
#find_package(OpenCV REQUIRED)
#find_package(Boost REQUIRED COMPONENTS system chrono thread)
#find_package(Eigen REQUIRED)

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if you package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
# important: common/include needs to be here if you have such a directory
catkin_package(
INCLUDE_DIRS
#	common/include
#	ros/include
LIBRARIES
CATKIN_DEPENDS
	${catkin_RUN_PACKAGES}
DEPENDS
)


###########
## Build ##
###########
## Specify additional locations of header files
## Your package locations should be listed before other locations
# important: common/include also needs to be here if you have it, order of including the coinor-packages relevant, not working when including in wrong order
include_directories(
#	common/include
#	ros/include
	${catkin_INCLUDE_DIRS}
)

# server for coverage checking
#add_executable(coverage_check_server
#	ros/src/coverage_check_server.cpp
#)
#target_link_libraries(coverage_check_server
#	${catkin_LIBRARIES} 
#	${OpenCV_LIBS}
#	${Boost_LIBRARIES}
#)
#add_dependencies(coverage_check_server ${catkin_EXPORTED_TARGETS})

#############
## Install ##
#############
# Mark executables and/or libraries for installation
#install(TARGETS room_exploration_server room_exploration_client
#	ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#	LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#	RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
#)

#uncomment this if you have a common-directory with header-files
install(DIRECTORY common/include/${PROJECT_NAME}/
	DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
	FILES_MATCHING PATTERN "*.h"
	PATTERN ".svn" EXCLUDE
)

#uncomment this if you have header-files in your project
install(DIRECTORY ros/include/${PROJECT_NAME}/
	DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
	FILES_MATCHING PATTERN "*.h"
	PATTERN ".svn" EXCLUDE
)

#install(DIRECTORY scripts
#	DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
#	PATTERN ".svn" EXCLUDE
#)

# uncomment this if you have launch files
#install(DIRECTORY ros/launch
#	DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/ros
#	PATTERN ".svn" EXCLUDE
#)

#install(DIRECTORY common/files
#	DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/common
#	PATTERN ".svn" EXCLUDE
#)
