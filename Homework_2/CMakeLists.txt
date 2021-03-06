# Main cmake file 
# Simple Model

cmake_minimum_required(VERSION 2.6)

# Main project name is ARMaker
project(HW2_Program)

# Make sure that glew32.lib and glfw.lib are in SDK/lib.
set(SDK_DIRECTORY "C:/Users/alofq/Documents/HCI-557-CG/SDK")

set(CFILE_PATH  "${SDK_DIRECTORY}/cmake/")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/" ${CMAKE_MODULE_PATH} "${SDK_DIRECTORY}/cmake/")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${SDK_DIRECTORY}")
#set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${SDK_DIRECTORY})

message(STATUS "Source directory ${CMAKE_SOURCE_DIR}")
message(STATUS "SDK directory ${SDK_DIRECTORY}")


# Find packages
FIND_PACKAGE(glew REQUIRED)
FIND_PACKAGE(glm REQUIRED)
FIND_PACKAGE(glfw3 REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)


# Include dirs
include_directories(${GLEW_INCLUDE_DIR})
include_directories(${GLFW3_INCLUDE_DIR})
include_directories(${GLM_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIR})

# Add all files to the configuration
file(GLOB HW2_Program_SRC
	hw2_main.cpp
    controls.cpp
    controls.h
    HCI557Common.cpp
    HCI557Common.h
	CoordSystem.cpp
	CoordSystem.h
)

# Create an executable
add_executable(HW2_Program ${HW2_Program_SRC})


# Add link directories
# Note required for this project
IF(WIN32)
	link_directories(${SDK_DIRECTORY}/lib)
ELSE(WIN32)
	link_directories(${SDK_DIRECTORY}/lib)
	link_directories(/usr/local/lib)
	link_directories(/usr/local/Cellar/glfw3/3.0.4/lib)
	link_directories(/usr/local/Cellar/glew/1.11.0/lib)
ENDIF(WIN32)


# Add libraries
target_link_libraries(HW2_Program ${GLEW_LIBRARY} ${GLFW3_LIBRARY} ${OPENGL_LIBRARIES} ${OPENGL_LIBRARIES} )
