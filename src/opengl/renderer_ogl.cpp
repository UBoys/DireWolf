#include <iostream>
#include "common/config.h"
#include "renderer_ogl.h"

#if defined(_WIN32)
#include "platform/rendercontext_ogl_win.h"
#endif

#include <string>
#include <iostream>
#include <vector>
#include <cassert>

#include "opengl_utils.h"

namespace {
    GLuint s_hardcoded_program_remove_me;
}

namespace dw {

void RendererOGL::Initialize(const RendererCaps& caps, const PlatformData& platformData) {
    std::cout << "DireWolf: Initializing OpenGL Renderer\n";

    // Initialize render context
#if defined(_WIN32)
    m_renderContext = std::make_unique<RenderContextWin>(platformData);
#endif

    // Load all static resources
	s_hardcoded_program_remove_me = opengl::utils::LoadShader("../../src/opengl/shaders/standard.vertex", "../../src/opengl/shaders/standard.fragment");
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);

	glUseProgram(s_hardcoded_program_remove_me);

 //   // An array of 3 vectors which represents 3 vertices
 //   static const GLfloat g_vertex_buffer_data[] = {
 //      -1.0f, -1.0f, 0.0f,
 //      1.0f, -1.0f, 0.0f,
 //      0.0f,  1.0f, 0.0f,
 //   };

	//GLuint vao;
	//glGenVertexArrays(1, &vao);
	//glBindVertexArray(vao);


 //   // This will identify our vertex buffer
 //   GLuint vertexbuffer;
 //   // Generate 1 buffer, put the resulting identifier in vertexbuffer
 //   glGenBuffers(1, &vertexbuffer);
 //   // The following commands will talk about our 'vertexbuffer' buffer
 //   glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
 //   // Give our vertices to OpenGL.
 //   glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

 //   glUseProgram(program);

 //   while (true) {
 //       glClear(GL_COLOR_BUFFER_BIT);

 //       // 1st attribute buffer : vertices
 //       glEnableVertexAttribArray(0);
 //       glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
 //       glVertexAttribPointer(
 //          0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
 //          3,                  // size
 //          GL_FLOAT,           // type
 //          GL_FALSE,           // normalized?
 //          0,                  // stride
 //          (void*)0            // array buffer offset
 //       );
 //       // Draw the triangle !
 //       glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
 //       glDisableVertexAttribArray(0);

 //       m_renderContext->SwapBuffers();
    //}
}

bool RendererOGL::CreateVertexBuffer(const GfxObject& object, uint32_t count) {
    // Create VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const uint32_t elementSize = sizeof(float) * 3; // TODO: Don't assume single float3 layout
    const uint32_t byteSize = count * elementSize;

    // Allocate buffer
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, byteSize, nullptr, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, /*layout*/3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));

    m_vertexBuffers.emplace(object, vao);
    return true;
}

void* RendererOGL::MapVertexBuffer(const GfxObject& object) {
    std::cout << "Mapping vertex buffer " << std::endl;
    const auto vertexBufferIt = m_vertexBuffers.find(object);
    assert(vertexBufferIt != m_vertexBuffers.end() && "Failed to find requested vertex buffer");
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferIt->second);
    void* result = glMapBufferRange(GL_ARRAY_BUFFER, 0, /*TODO: remove hardcode*/ sizeof(float) * 3, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
    return result;
}

void RendererOGL::UnmapVertexBuffer(const GfxObject& object) {
    std::cout << "Unmapping vertex buffer " << std::endl;
    const auto vertexBufferIt = m_vertexBuffers.find(object);
    assert(vertexBufferIt != m_vertexBuffers.end() && "Failed to find requested vertex buffer");
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferIt->second);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void RendererOGL::BindVertexBuffer(BindVertexBufferCommandData* data) {
    std::cout << "Binding vertex buffer " << std::endl;
    const auto vertexBufferIt = m_vertexBuffers.find(*data->object);

    // TODO: Make our own asserts DW_ASSERT() or something.
    assert(vertexBufferIt != m_vertexBuffers.end() && "Failed to find requested vertex buffer");
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferIt->second);
}

bool RendererOGL::CreatePipelineState(const GfxObject& object, const PipelineState& pipelineState) {
	// TODO: Create program state here which includes the shader
    return false;
}

void RendererOGL::Draw(DrawCommandData* data) {
	glClear(GL_COLOR_BUFFER_BIT);
    std::cout << "Drawing " << data->count << std::endl;
	glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
	m_renderContext->SwapBuffers();
}

void RendererOGL::BindPipelineState(BindPipelineStateCommandData* data) {
    std::cout << "Binding pipeline state" << std::endl;
    // TODO: Bind all PSO states. Hardcode our only program for now
    //glUseProgram(s_hardcoded_program_remove_me);
}

void RendererOGL::Render(const std::vector<RenderCommand>& commandBuffer) {
    for (RenderCommand command : commandBuffer) {
        switch (command.type) {
            case BindVertexBufferCommand:
                BindVertexBuffer(static_cast<BindVertexBufferCommandData*>(command.data));
                break;
            case BindPipelineStateCommand:
                BindPipelineState(static_cast<BindPipelineStateCommandData*>(command.data));
                break;
            case DrawCommand:
                Draw(static_cast<DrawCommandData*>(command.data));
                break;
            default:
                std::cerr << "Unsupported rendering command!" << std::endl;
        }
    }
}


}  // namespace dw
