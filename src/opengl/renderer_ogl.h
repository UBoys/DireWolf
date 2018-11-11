#pragma once

#include "irenderer.h"

namespace dwf {

struct PlatformData;
struct InitData;
class RendererOGL final : public IRenderer {
public:
    virtual void Initialize(const RendererCaps& caps, const PlatformData& data) override;

    virtual GfxObject CreateVertexBuffer(const uint32_t count) override { return GfxObject(); }
    virtual GfxObject CreateIndexBuffer(const uint32_t count) override { return GfxObject(); }
    virtual GfxObject CreatePipelineState(const PipelineState& state) override { return GfxObject(); }
    virtual GfxObject CreateTexture(const TextureDescription& description,
                                    const std::vector<void*>& data) override { return GfxObject();}
    virtual GfxObject CreateSamplerState(const SamplerDescription& description) override { return GfxObject(); }

    virtual void* MapVertexBuffer(const GfxObject& handle) override { return nullptr; };
    virtual void* MapIndexBuffer(const GfxObject& handle) override { return nullptr; };
    virtual void UnmapVertexBuffer(const GfxObject& handle, const uint32_t count) override {};
    virtual void UnmapIndexBuffer(const GfxObject& handle, const uint32_t count) override {};

    virtual void DestroyVertexBuffer(const GfxObject& handle) override {};
    virtual void DestroyIndexBuffer(const GfxObject& handle) override {};
    virtual void DestroyTexture(const GfxObject& handle) override {};
    virtual void DestroyPipelineState(const GfxObject& handle) override {};
    virtual void DestroySamplerState(const GfxObject& hanele) override {};

    // Actual rendering commands that operate on updated and ready resources.
    virtual void Render(const std::vector<RenderCommand>& commandBuffer){};
};

}  // namespace dwf
