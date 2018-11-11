
add_executable(example_vulkan example_vulkan.cpp)
target_link_libraries(example_vulkan direwolf)
target_include_directories(example_vulkan PUBLIC ../include)

if (DIREWOLF_VULKAN_ENABLED)
  target_compile_definitions(example_vulkan PRIVATE DW_VULKAN_ENABLED=1)
endif (DIREWOLF_VULKAN_ENABLED)
