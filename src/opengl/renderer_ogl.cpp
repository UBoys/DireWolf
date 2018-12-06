#include <iostream>
#include "common/config.h"
#include "renderer_ogl.h"

#if defined(_WIN32)
  #include "platform/rendercontext_ogl_win.h"
#elif defined(__APPLE__)
  #include "platform/rendercontext_ogl_osx.h"
#endif

#include <string>
#include <iostream>
#include <vector>
#include <cassert>

// TODO: I just put stuff here meanwhile
#include "opengl_utils.h"
#include "utils/logger.h"

namespace {
    GLuint s_HARDCODED_PROGRAM_REMOVE_ME; // TODO: Remove
    const uint32_t HARDCODED_VERTEX_ELEMENT_SIZE = sizeof(float) * 4; // TODO: Remove
}

namespace dw {

void RendererOGL::Initialize(const RendererCaps& caps, const PlatformData& platformData) {
    LOGD("Initializing OpenGL Renderer");

    // Initialize render context
#if defined(_WIN32)
    m_renderContext = std::make_unique<RenderContextWin>(platformData);
#elif defined(__APPLE__)
    m_renderContext = std::make_unique<RenderContextOSX>(platformData);
#endif

    // Load all static resources
    s_HARDCODED_PROGRAM_REMOVE_ME = opengl::utils::LoadShader("../../src/opengl/shaders/standard.vertex", "../../src/opengl/shaders/standard.fragment");
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f); // Display ugly green color

    // TODO: Part of creating the pipeline state
    glUseProgram(s_HARDCODED_PROGRAM_REMOVE_ME);
}

// TODO: Pass in what desired layout in this function t.ex float4/pos, float4/color, float4/somethingelse. Right now assume float4 positions
bool RendererOGL::CreateVertexBuffer(const GfxObject& object, uint32_t count) {
    // TODO: Do not assume a single VAO/layout
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const uint32_t elementSize = HARDCODED_VERTEX_ELEMENT_SIZE;
    const uint32_t byteSize = count * elementSize;

    // Allocate buffer
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    glBufferData(GL_ARRAY_BUFFER, byteSize, nullptr, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, elementSize, static_cast<void*>(0));

    m_vertexBuffers.emplace(object, vertexBuffer); // TODO: Store other state here such as vao, count, base vertex, etc 
    return true;
}

void* RendererOGL::MapVertexBuffer(const GfxObject& object) {
    LOGD("Mapping vertex buffer");
    const auto vertexBufferIt = m_vertexBuffers.find(object);
    assert(vertexBufferIt != m_vertexBuffers.end() && "Failed to find requested vertex buffer");
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferIt->second);
    /*TODO: Remove hardcoded count*/
    void* result = glMapBufferRange(GL_ARRAY_BUFFER, 0, HARDCODED_VERTEX_ELEMENT_SIZE * 3, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
    return result;
}

void RendererOGL::UnmapVertexBuffer(const GfxObject& object) {
    LOGD("Unmapping vertex buffer");
    const auto vertexBufferIt = m_vertexBuffers.find(object);
    assert(vertexBufferIt != m_vertexBuffers.end() && "Failed to find requested vertex buffer");
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferIt->second);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void RendererOGL::BindVertexBuffer(BindVertexBufferCommandData* data) {
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
    glDrawArrays(GL_TRIANGLES, data->startVertex, data->count);
}

void RendererOGL::BindPipelineState(BindPipelineStateCommandData* data) {
    LOGD("Binding pipeline state");
    // TODO: Bind all PSO states. Hardcode our only program for now
    //glUseProgram(s_HARDCODED_PROGRAM_REMOVE_ME);
}

void RendererOGL::Render(const std::vector<RenderCommand>& commandBuffer) {
    glClear(GL_COLOR_BUFFER_BIT); // TODO: Separate clear command?
    for (const RenderCommand& command : commandBuffer) {
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
    m_renderContext->SwapBuffers();
}

}  // namespace dw
