message(STATUS "\nAdding examples" )

# Always include GLFW for now
add_subdirectory(ext/glfw)
add_subdirectory(ext/glm)

add_executable(example_vulkan example_vulkan.cpp)
target_link_libraries(example_vulkan ${PROJECT_NAME} glfw)
target_include_directories(example_vulkan PUBLIC ../include)

add_executable(example_triangle example_triangle/main.cpp)
target_link_libraries(example_triangle ${PROJECT_NAME} glfw)
target_include_directories(example_triangle PUBLIC ../include)

add_executable(example_spinning_cube example_spinning_cube/main.cpp)
target_link_libraries(example_spinning_cube ${PROJECT_NAME} glfw)
target_include_directories(example_spinning_cube PUBLIC ../include ext/glm)

if (DIREWOLF_VULKAN_ENABLED)
  target_compile_definitions(example_vulkan PRIVATE DW_VULKAN_ENABLED=1)
endif (DIREWOLF_VULKAN_ENABLED)
