cmake_minimum_required(VERSION 3.9.2)
project(DireWolf DESCRIPTION "Device Independent Rendering Engine with Other Lively Features")

message(STATUS "\nGenerating ${PROJECT_NAME}")

# Compilation flags
if(MSVC)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /std:c++17")
else(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++17")
endif(MSVC)

option(DIREWOLF_BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(DIREWOLF_BUILD_EXAMPLES "Should the examples folder be added?" OFF) # HACK/TODO: Don't want to add GLFW dep to travis. Do we want to build samples by default?
option(DIREWOLF_VULKAN_ENABLED "Should we include vulkan features?" OFF)
option(DIREWOLF_OPENGL_ENABLED "Should we include OpenGL features?" ON)

set(lib_type STATIC)
if (DIREWOLF_BUILD_SHARED_LIBS)
  set(lib_type SHARED)
endif()

message(STATUS "Creating DireWolf as a ${lib_type} library")

# Add library and mandatory sourcefiles
add_library(${PROJECT_NAME}
  ${lib_type}
    src/renderengine.cpp
    include/direwolf/renderengine.h
)

if (DIREWOLF_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if (DIREWOLF_OPENGL_ENABLED)
  message(STATUS "\nIncluding OpenGL module to ${PROJECT_NAME}")

  add_definitions(-DGLEW_STATIC)
  add_subdirectory(ext/glew-cmake)

  target_include_directories(${PROJECT_NAME}
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/ext/glew-cmake/include
      ${CMAKE_CURRENT_SOURCE_DIR}/ext/glew-cmake/src
  )

  target_sources(${PROJECT_NAME}
    PRIVATE
      src/opengl/renderer_ogl.cpp
      src/opengl/irendercontext_ogl.h
      src/opengl/platform/rendercontext_ogl_win.cpp
      src/opengl/platform/rendercontext_ogl_win.h
  )

  target_link_libraries(${PROJECT_NAME} libglew_static)
endif (DIREWOLF_OPENGL_ENABLED)

if (DIREWOLF_VULKAN_ENABLED)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/vulkan)
endif (DIREWOLF_VULKAN_ENABLED)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

message(STATUS "\nSetup of " ${PROJECT_NAME} " finished!\n")
