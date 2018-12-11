#pragma once

#include "irenderer.h"
#include "opengl/irendercontext_ogl.h"
#include <map>
#include <memory>

#if defined(_WIN32)
  #include <GL/glew.h>
#elif defined(__APPLE__)
  #include <OpenGL/gl3.h>
  #include <OpenGL/glext.h>
#endif

namespace dw {

struct PlatformData;
struct InitData;

class RendererOGL final : public IRenderer {
public:
    virtual void Initialize(const RendererCaps& caps, const PlatformData& data) override;

    virtual bool CreateConstantBuffer(const GfxObject& object, uint32_t size) override;
    virtual bool CreateVertexBuffer(const GfxObject& object, uint32_t count) override;
    virtual bool CreateIndexBuffer(const GfxObject& object, uint32_t count) override { return false; }
    virtual bool CreatePipelineState(const GfxObject& object, const PipelineState& state) override;
    virtual bool CreateSamplerState(const GfxObject& object, const SamplerDescription& description) override { return false; }
    virtual bool CreateTexture(const GfxObject& object, const TextureDescription& description, void* data, uint32_t dataLength) override;

    virtual void* MapConstantBuffer(const GfxObject& handle) override;
    virtual void* MapVertexBuffer(const GfxObject& handle) override;
    virtual void* MapIndexBuffer(const GfxObject& handle) override { return nullptr; };
    virtual void UnmapVertexBuffer(const GfxObject& handle) override;
    virtual void UnmapIndexBuffer(const GfxObject& handle, const uint32_t count) override {};
    virtual void UnmapConstantBuffer(const GfxObject& handle) override;

    virtual void DestroyVertexBuffer(const GfxObject& handle) override {};
    virtual void DestroyIndexBuffer(const GfxObject& handle) override {};
    virtual void DestroyTexture(const GfxObject& handle) override {};
    virtual void DestroyPipelineState(const GfxObject& handle) override {};
    virtual void DestroySamplerState(const GfxObject& hanele) override {};

    // Actual rendering commands that operate on updated and ready resources.
    virtual void Render(const std::vector<RenderCommand>& commandBuffer) override;

private:
    void bindTextures(BindTexturesCommandData* data) const; 
    void bindConstantBuffer(BindConstantBufferCommandData* data, uint8_t slot) const;
    void bindVertexBuffer(BindVertexBufferCommandData* data) const;
    void bindPipelineState(BindPipelineStateCommandData* data) const;
    void draw(DrawCommandData* data) const;

    std::map<GfxObject, GLuint> m_textures;
    std::map<GfxObject, GLuint> m_vertexBuffers;
    std::map<GfxObject, GLuint> m_constantBuffers;
    std::unique_ptr<IRenderContextOGL> m_renderContext;
};

}  // namespace dw
