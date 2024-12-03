cmake_minimum_required(VERSION 3.16)

# Set the project name and languages
project(cs1230_final_lights_camera_explosion LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# no optimizations for debug build
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

# Include the current directory for header files
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Allows you to include files from within those directories, without prefixing their filepaths
include_directories(src)

# enable glm swizzling
add_definitions(-DGLM_FORCE_SWIZZLE)

# Specifies .cpp and .h files to be passed to the compiler
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/main.cpp
    src/utils/shaderloader.h
    src/realtime.cpp
    src/realtime.h
    src/utils/sceneparser.cpp
    src/utils/sceneparser.h
    src/utils/scenedata.h
    src/utils/scenefilereader.cpp
    src/utils/scenefilereader.h
    src/settings.cpp
    src/settings.h
    src/mainwindow.cpp
    src/mainwindow.h
    src/objects/realtimeobject.cpp
    src/objects/realtimeobject.h
    src/meshes/primitivemesh.cpp
    src/meshes/primitivemesh.h
    src/meshes/conemesh.cpp
    src/meshes/conemesh.h
    src/meshes/cylindermesh.cpp
    src/meshes/cylindermesh.h
    src/meshes/cubemesh.cpp
    src/meshes/cubemesh.h
    src/meshes/spheremesh.cpp
    src/meshes/spheremesh.h
    src/realtimescene.cpp
    src/realtimescene.h
    src/camera.cpp
    src/camera.h
    src/aabb.h
    src/objects/staticobject.cpp
    src/objects/staticobject.h
    src/objects/collisionobject.cpp
    src/objects/collisionobject.h
    src/objects/collisionobject.cpp
    src/objects/collisionobject.h
    src/objects/staticobject.cpp
    src/objects/staticobject.h
    src/aabb.cpp
    src/objects/playerobject.cpp
    src/objects/playerobject.h
)

# GLM: this creates its library and allows you to `#include "glm/..."`
add_subdirectory(glm)

# GLEW: this creates its library and allows you to `#include "GL/glew.h"`
add_library(StaticGLEW STATIC glew/src/glew.c)
include_directories(${PROJECT_NAME} PRIVATE glew/include)

# nlohmann_json
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(nlohmann_json)

# find OpenGL
find_package(OpenGL REQUIRED)

# Find pkg-config
find_package(PkgConfig REQUIRED)

# Use pkg-config to find GLFW
pkg_check_modules(GLFW REQUIRED glfw3)

# Include GLFW directories
include_directories(${GLFW_INCLUDE_DIRS})
link_directories(${GLFW_LIBRARY_DIRS})
add_definitions(${GLFW_CFLAGS_OTHER})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glfw
    StaticGLEW
    nlohmann_json::nlohmann_json
)

# GLEW: this provides support for Windows (including 64-bit)
if (WIN32)
  add_compile_definitions(GLEW_STATIC)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    opengl32
    glu32
  )
endif()

# Set this flag to silence warnings on Windows
if (MSVC OR MSYS OR MINGW)
  set(CMAKE_CXX_FLAGS "-Wno-volatile")
endif()
# Set this flag to silence warnings on MacOS
if (APPLE)
  set(CMAKE_CXX_FLAGS "-Wno-deprecated-volatile")
endif()
