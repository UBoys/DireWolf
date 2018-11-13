# Always include GLFW for now
add_subdirectory(ext/glfw)

add_executable(example_vulkan example_vulkan.cpp)
target_link_libraries(example_vulkan direwolf glfw)
target_include_directories(example_vulkan PUBLIC ../include)

add_executable(example_opengl example_opengl.cpp)
target_link_libraries(example_opengl direwolf glfw)
target_include_directories(example_opengl PUBLIC ../include)

if (DIREWOLF_VULKAN_ENABLED)
  target_compile_definitions(example_vulkan PRIVATE DW_VULKAN_ENABLED=1)
endif (DIREWOLF_VULKAN_ENABLED)
