# Homework 4 Problem 1

cmake_minimum_required(VERSION 2.6)

project(HW4_P2_Program)

# Make sure that glew32.lib and glfw.lib are in SDK/lib.
set(SDK_DIRECTORY "C:/Users/Aloshi/Documents/Visual Studio 2015/Projects/HCI-557-CG/SDK")
add_subdirectory(../common common)

# Add all files to the configuration
file(GLOB HW4_Program_SRC
  hw4_p2.cpp
)

# Create an executable
add_executable(HW4_P2_Program ${HW4_Program_SRC})
target_link_libraries(HW4_P2_Program HW4_Common)
