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

#define BUFFER_OFFSET(i) (static_cast<uint8_t*>(nullptr) + i)

namespace {
    GLuint s_HARDCODED_PROGRAM_REMOVE_ME;
    GLuint s_BUFFER_SLOT = 1; // Arbitrary, per constant buffer

    void CheckOpenGLError(const char *stmt, const char *fname, int line) {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "OOPS!";
            printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
            abort();
        }
    }
}

#define _DEBUG // Hack

#ifdef _DEBUG
  #define GL_CHECK(stmt)                             \
    do {                                             \
      stmt;                                        \
      CheckOpenGLError(#stmt, __FILE__, __LINE__); \
    } while (0)
#else
  #define GL_CHECK(stmt) stmt
#endif

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
    s_HARDCODED_PROGRAM_REMOVE_ME = opengl::utils::LoadShader("../../examples/example_textured_cube/standard.vertex", "../../examples/example_textured_cube/standard.fragment");

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f); // Display ugly green color

    const uint8_t blockIndex = glGetUniformBlockIndex(s_HARDCODED_PROGRAM_REMOVE_ME, "ShaderConstants");
    GL_CHECK(glUniformBlockBinding(s_HARDCODED_PROGRAM_REMOVE_ME, blockIndex, s_BUFFER_SLOT));

    // TODO: Part of creating the pipeline state
    GL_CHECK(glUseProgram(s_HARDCODED_PROGRAM_REMOVE_ME));
}

bool RendererOGL::CreateTexture(const GfxObject& object, const TextureDescription& description, void* data, uint32_t dataLength) {
    GLuint textureId;
    glGenTextures(1, &textureId);

    // TODO: Move to some util/helper
    GLuint textureFormat;
    switch (description.format) {
        case R8G8B8A8_UNORM:
            textureFormat = GL_RGB;
            break;
        default:
            assert(false && "Unimplemented texture format!\n");
            break;
    }

    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, description.width, description.height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    // TODO: This is really part of the sampler state
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // TODO: Mipmaps should not be hardcoded
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    m_textures.emplace(object, textureId);
    return true;
}


bool RendererOGL::CreateConstantBuffer(const GfxObject& object, uint32_t size) {
    GLuint constantBuffer;
    GL_CHECK(glGenBuffers(1, &constantBuffer));
    LOGD("Creating constant buffer with id " + std::to_string(constantBuffer) + " and size " + std::to_string(size));

    GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, constantBuffer));
    GL_CHECK(glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
    m_constantBuffers.emplace(object, constantBuffer);
    return true;
}

// TODO: How to handle vertex layout? Right now only support single float4:position/color
bool RendererOGL::CreateVertexBuffer(const GfxObject& object, uint32_t count) {
    GLuint vao;
    GL_CHECK(glGenVertexArrays(1, &vao));
    GL_CHECK(glBindVertexArray(vao));

    const uint32_t elementSize = 4 * sizeof(float) * 2; // TODO/hack. This is an assumption, i.e size of each struct { vec4 position; vec4 color; }
    const uint32_t byteSize = count * elementSize;

    GLuint vertexBuffer;
    GL_CHECK(glGenBuffers(1, &vertexBuffer));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, byteSize, nullptr, GL_STATIC_DRAW));

    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glEnableVertexAttribArray(1));
    GL_CHECK(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, elementSize, BUFFER_OFFSET(0)));
    GL_CHECK(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, elementSize, BUFFER_OFFSET(sizeof(float) * 4)));

    m_vertexBuffers.emplace(object, vertexBuffer);
    return true;
}

void* RendererOGL::MapConstantBuffer(const GfxObject& object) {
    const auto constBufferIt = m_constantBuffers.find(object);
    //LOGD("Mapping const buffer with id " + std::to_string(constBufferIt->second));
    assert(constBufferIt != m_constantBuffers.end() && "Failed to find requested constant buffer");
    GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, constBufferIt->second));
    void* mappedBuffer = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    return mappedBuffer;
}

void RendererOGL::UnmapConstantBuffer(const GfxObject& object) {
    const auto constBufferIt = m_constantBuffers.find(object);
    //LOGD("Unmapping const buffer with id " + std::to_string(constBufferIt->second));
    assert(constBufferIt != m_constantBuffers.end() && "Failed to find requested constant buffer");
    GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, constBufferIt->second));
    GL_CHECK(glUnmapBuffer(GL_UNIFORM_BUFFER));
}

void* RendererOGL::MapVertexBuffer(const GfxObject& object) {
    LOGD("Mapping vertex buffer");
    const auto vertexBufferIt = m_vertexBuffers.find(object);
    assert(vertexBufferIt != m_vertexBuffers.end() && "Failed to find requested vertex buffer");
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferIt->second));
    void *result = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    return result;
}

void RendererOGL::UnmapVertexBuffer(const GfxObject& object) {
    LOGD("Unmapping vertex buffer");
    const auto vertexBufferIt = m_vertexBuffers.find(object);
    assert(vertexBufferIt != m_vertexBuffers.end() && "Failed to find requested vertex buffer");
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferIt->second));
    GL_CHECK(glUnmapBuffer(GL_ARRAY_BUFFER));
}

bool RendererOGL::CreatePipelineState(const GfxObject& object, const PipelineState& pipelineState) {
    // TODO: Create program state here which includes the shader
    return false;
}

void RendererOGL::Render(const std::vector<RenderCommand>& commandBuffer) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // TODO: Separate clear command?
    for (const RenderCommand& command : commandBuffer) {
        switch (command.type) {
            case BIND_TEXTURES:
                bindTextures(static_cast<BindTexturesCommandData*>(command.data));
                break;
            case BIND_VERTEX_BUFFER:
                bindVertexBuffer(static_cast<BindVertexBufferCommandData*>(command.data));
                break;
            case BIND_PIPELINE_STATE:
                bindPipelineState(static_cast<BindPipelineStateCommandData*>(command.data));
                break;
            case BIND_CONSTANT_BUFFER:
                bindConstantBuffer(static_cast<BindConstantBufferCommandData*>(command.data), 0);
                break;
            case DRAW:
                draw(static_cast<DrawCommandData*>(command.data));
                break;
            default:
                std::cerr << "Unsupported rendering command!" << std::endl;
        }
    }
    m_renderContext->SwapBuffers();
}

void RendererOGL::bindTextures(BindTexturesCommandData* data) const {
    uint8_t texUnit = 0;
    for (auto&& object : data->objects) {
        const auto it = m_textures.find(*object);
        assert(it != m_textures.end() && "Asked for non-existent texture\n");
        // Texture unit binds to samplers and texture data
        glActiveTexture(GL_TEXTURE0 + texUnit++);
        glBindTexture(GL_TEXTURE_2D, it->second);
    }
}

void RendererOGL::bindPipelineState(BindPipelineStateCommandData* data) const {
    LOGD("Binding pipeline state");
    // TODO: Bind all PSO states. Hardcode our only program for now
    //glUseProgram(s_HARDCODED_PROGRAM_REMOVE_ME);
}

void RendererOGL::bindConstantBuffer(BindConstantBufferCommandData* data, uint8_t slot) const {
    const auto constBufferIt = m_constantBuffers.find(*data->object);
    assert(constBufferIt != m_constantBuffers.end() && slot == 0 && "Failed to find requested constant buffer");
    GL_CHECK(glBindBufferBase(GL_UNIFORM_BUFFER, s_BUFFER_SLOT, constBufferIt->second));
}

void RendererOGL::bindVertexBuffer(BindVertexBufferCommandData* data) const {
    const auto vertexBufferIt = m_vertexBuffers.find(*data->object);
    assert(vertexBufferIt != m_vertexBuffers.end() && "Failed to find requested vertex buffer");
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferIt->second));
}

void RendererOGL::draw(DrawCommandData* data) const {
    GL_CHECK(glDrawArrays(GL_TRIANGLES, data->startVertex, data->count));
}

}  // namespace dw
