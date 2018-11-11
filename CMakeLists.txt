cmake_minimum_required(VERSION 3.9.2)
message("Creating project DireWolf")
project(DireWolf DESCRIPTION "Device Independent Rendering Engine with Other Lively Features")

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(DIREWOLF_BUILD_EXAMPLES "Should the examples folder be added?" ON)
option(DIREWOLF_USE_VERBOSE_VULKAN_LOG "Should the Direwolf Vulkan module print verbose info messages?" ON)
option(DIREWOLF_VULKAN_ENABLED "Should we include vulkan features?" OFF)
option(DIREWOLF_OPENGL_ENABLED "Should we include OpenGL features?" ON)

set(lib_type STATIC)
if (BUILD_SHARED_LIBS)
  set(lib_type SHARED)
endif()

message("Creating DireWolf as a ${lib_type} library")

# Add library and mandatory sourcefiles
add_library(direwolf
  ${lib_type}
    src/renderengine.cpp
    include/direwolf/renderengine.h
    # TODO: this shouldn't be here.. remove when vulkan stuff is included in the direwolf wrappers
    include/direwolf/vulkan/vulkansetup.h
)

if (DIREWOLF_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if (DIREWOLF_OPENGL_ENABLED)
  message("Including OpenGL")

  target_sources(direwolf PRIVATE src/opengl/renderer_ogl.cpp)
endif (DIREWOLF_OPENGL_ENABLED)

if (DIREWOLF_VULKAN_ENABLED)
  message("Including Vulkan")

  find_package(Vulkan REQUIRED)
  if (Vulkan_FOUND)
    message("Found Vulkan")
  endif (Vulkan_FOUND)

  target_include_directories(direwolf
    #TODO this should be PRIVATE.. change when vulkan stuff is included in the direwolf wrappers
    PUBLIC
      ${Vulkan_INCLUDE_DIR})

  target_sources(direwolf
    PRIVATE
      # TODO: this should be here src/vulkan/vulkansetup.h.. add when vulkan stuff is included in the direwolf wrappers
      src/vulkan/listofvulkanfunctions.inl
      src/vulkan/vulkanfunctions.cpp
      src/vulkan/vulkanfunctions.h
      src/vulkan/vulkansetup.cpp
      src/vulkan/vulkanutils.h
      src/vulkan/vulkanutils.inl
      src/vulkan/vulkanutils.cpp
  )

  target_compile_definitions(direwolf
    PRIVATE
      # DW_VULKAN_ENABLED=1
      VK_NO_PROTOTYPES=1
  )

  if (DIREWOLF_USE_VERBOSE_VULKAN_LOG)
      target_compile_definitions(direwolf
        PRIVATE
          DW_VERBOSE_LOG_VK = 1
      )
  endif(DIREWOLF_USE_VERBOSE_VULKAN_LOG)

endif (DIREWOLF_VULKAN_ENABLED)

target_include_directories(direwolf
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
