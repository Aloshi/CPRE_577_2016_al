# Main cmake file 

cmake_minimum_required(VERSION 2.6)

project(HW4_Common)

# Make sure that glew32.lib and glfw.lib are in SDK/lib.
#set(SDK_DIRECTORY "C:/Users/Aloshi/Documents/Visual Studio 2015/Projects/HCI-557-CG/SDK")

set(CFILE_PATH  "${SDK_DIRECTORY}/cmake/")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/" ${CMAKE_MODULE_PATH} "${SDK_DIRECTORY}/cmake/")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${SDK_DIRECTORY}")

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
include_directories(include)

# Add all files to the configuration
set(HW4_Common_INC
  include/openglpp/LoadBMP.h
  include/openglpp/GLException.h
  include/openglpp/Window.h
  include/openglpp/Shader.h
  include/openglpp/VertexBufferObject.h
  include/openglpp/VertexArrayObject.h
  include/openglpp/Camera.h
  include/openglpp/Transform.h
  include/openglpp/Texture.h
  include/openglpp/generators/SphereGenerator.h
  include/openglpp/Material.h
  include/openglpp/Mesh.h
  include/openglpp/Object.h
)

set(HW4_Common_SRC
  src/openglpp/LoadBMP.cpp

  src/openglpp/Window.cpp
  src/openglpp/Shader.cpp
  src/openglpp/VertexBufferObject.cpp
  src/openglpp/VertexArrayObject.cpp
  src/openglpp/Camera.cpp
  src/openglpp/Transform.cpp
  src/openglpp/Texture.cpp

  src/openglpp/generators/SphereGenerator.cpp

  src/openglpp/Mesh.cpp
)

# Create statically linked library
add_library(HW4_Common STATIC ${HW4_Common_SRC} ${HW4_Common_INC})
target_include_directories(HW4_Common PUBLIC ${GLEW_INCLUDE_DIR} ${GLFW3_INCLUDE_DIR} ${GLM_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
target_include_directories(HW4_Common PUBLIC ./include)

target_link_libraries(HW4_Common ${GLEW_LIBRARY} ${GLFW3_LIBRARY} ${OPENGL_LIBRARIES} ${OPENGL_LIBRARIES})