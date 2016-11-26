# Main cmake file 
# Simple Model

cmake_minimum_required(VERSION 2.6)

# Main project name is ARMaker
project(trafficsim)

# Make sure that glew32.lib and glfw.lib are in SDK/lib.
#set(SDK_DIRECTORY "C:/Users/Aloshi/Documents/Visual Studio 2015/Projects/HCI-557-CG/SDK")
set(CMAKE_BUILD_TYPE debug)

set(CFILE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} "${SDK_DIRECTORY}/cmake/")
#set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${SDK_DIRECTORY}" "${CMAKE_SOURCE_DIR}/cmake")
#set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${SDK_DIRECTORY})

message(STATUS "Source directory ${CMAKE_SOURCE_DIR}")
message(STATUS "SDK directory ${SDK_DIRECTORY}")


# Find packages
FIND_PACKAGE(glm REQUIRED)
FIND_PACKAGE(glfw3 REQUIRED)
FIND_PACKAGE(glew REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)


# Include dirs
include_directories(${GLEW_INCLUDE_DIR})
include_directories(${GLFW3_INCLUDE_DIR})
include_directories(${GLM_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add all files to the configuration
set(trafficsim_INC
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
  include/openglpp/generators/PlaneGenerator.h
  include/openglpp/Material.h
  include/openglpp/Mesh.h
  include/openglpp/Object.h
  include/openglpp/LoadObj.h
  include/openglpp/tiny_obj_loader.h
  include/openglpp/Debug.h

  include/Util.h
  include/CameraController.h
  include/Event.h

  include/Spline.h
  include/RoadMesh.h
)

set(trafficsim_SRC
  src/openglpp/LoadBMP.cpp

  src/openglpp/Window.cpp
  src/openglpp/Shader.cpp
  src/openglpp/VertexBufferObject.cpp
  src/openglpp/VertexArrayObject.cpp
  src/openglpp/Camera.cpp
  src/openglpp/Transform.cpp
  src/openglpp/Texture.cpp
  src/openglpp/generators/SphereGenerator.cpp
  src/openglpp/generators/PlaneGenerator.cpp
  src/openglpp/Mesh.cpp
  src/openglpp/tiny_obj_loader.cpp
  src/openglpp/LoadObj.cpp
  src/openglpp/Debug.cpp

  src/main.cpp
  src/CameraController.cpp

  src/RoadMesh.cpp
)

# Create an executable
add_executable(trafficsim ${trafficsim_INC} ${trafficsim_SRC})

# Add link directories
# Note required for this project
IF(WIN32)
	link_directories(${SDK_DIRECTORY}/lib)
ELSE(WIN32)
	link_directories(${SDK_DIRECTORY}/lib)
	link_directories(/usr/local/lib)
ENDIF(WIN32)


# Add libraries
target_link_libraries(trafficsim ${GLEW_LIBRARY} ${GLFW3_LIBRARY} ${OPENGL_LIBRARIES} ${OPENGL_LIBRARIES})
target_compile_features(trafficsim PRIVATE cxx_override cxx_lambdas)
