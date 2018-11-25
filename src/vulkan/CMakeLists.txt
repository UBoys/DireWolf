cmake_minimum_required(VERSION 3.9.2)

message(STATUS "\nIncluding Vulkan module to ${PROJECT_NAME}")

option(DIREWOLF_VULKAN_USE_VERBOSE_LOG "Should the Direwolf Vulkan module print verbose info messages?" ON)

find_package(Vulkan REQUIRED)
if (Vulkan_FOUND)
  message(STATUS "Found Vulkan")
endif (Vulkan_FOUND)

target_include_directories(${PROJECT_NAME}
  #TODO this should be PRIVATE.. change when vulkan stuff is included in the direwolf wrappers
  PUBLIC
    ${Vulkan_INCLUDE_DIRS}
)

target_sources(${PROJECT_NAME}
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include/direwolf/vulkan/vulkancommons.h
    ${PROJECT_SOURCE_DIR}/include/direwolf/vulkan/vulkansetup.h
  PRIVATE
    # TODO: this should be here src/vulkan/vulkansetup.h.. add when vulkan stuff is included in the direwolf wrappers
    src/vulkan/listofvulkanfunctions.inl
    src/vulkan/vulkancommons.cpp
    src/vulkan/vulkanfunctions.cpp
    src/vulkan/vulkanfunctions.h
    src/vulkan/vulkansetup.cpp
    src/vulkan/vulkanutils.h
    src/vulkan/vulkanutils.inl
    src/vulkan/vulkanutils.cpp
)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
    VK_NO_PROTOTYPES=1
)

if (DIREWOLF_VULKAN_USE_VERBOSE_LOG)
  target_compile_definitions(${PROJECT_NAME}
    PRIVATE
      DW_VERBOSE_LOG_VK=1
  )
endif(DIREWOLF_VULKAN_USE_VERBOSE_LOG)
