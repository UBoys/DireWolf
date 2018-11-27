message(STATUS "\nIncluding OpenGL module to ${PROJECT_NAME}")

target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${PROJECT_SOURCE_DIR}/ext/glew-cmake/include
    ${PROJECT_SOURCE_DIR}/ext/glew-cmake/src
)

target_sources(${PROJECT_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer_ogl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/irendercontext_ogl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/rendercontext_ogl_win.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/rendercontext_ogl_win.h
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/rendercontext_ogl_osx.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/rendercontext_ogl_osx.h
)
