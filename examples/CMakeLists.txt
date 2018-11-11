
add_executable(helloworld_renderer helloworldrenderer.cpp)
target_link_libraries(helloworld_renderer direwolf)
target_include_directories(helloworld_renderer PUBLIC ../include)

if (DIREWOLF_VULKAN_ENABLED)
  target_compile_definitions(helloworld_renderer PRIVATE DW_VULKAN_ENABLED=1)
endif (DIREWOLF_VULKAN_ENABLED)
