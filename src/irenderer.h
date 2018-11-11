#pragma once

#include <vector>

namespace dwf {

enum RenderCommand {
    SetPipelineState,
    SetVertexBuffer,
    SetIndexBuffer,
    SetTextures,
    SetSamplers,
    DrawIndexed
};

struct RendererCaps {
    void* allocator;
    bool debug;
};

struct PlatformData;
struct InitData;
struct TextureDescription {};
struct SamplerDescription {};
// Should contain rasterizer state, depth stencil state, blend state and shader
struct PipelineState {};
// TODO: Constant buffers

// Handle for each renderer resource.
// All rendering resources are owned by the renderer and should not be coupled with client code
struct GfxObject {
    bool IsValid() const { return objectId != 0; }
    uint32_t objectId = 0;
};

class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual void Initialize(const RendererCaps& caps, const PlatformData& platformData) = 0;

    virtual GfxObject CreateVertexBuffer(const uint32_t count) = 0;
    virtual GfxObject CreateIndexBuffer(const uint32_t count) = 0;
    virtual GfxObject CreatePipelineState(const PipelineState& state) = 0;
    virtual GfxObject CreateTexture(const TextureDescription& description, const std::vector<void*>& data) = 0;
    virtual GfxObject CreateSamplerState(const SamplerDescription& description) = 0;

    virtual void* MapVertexBuffer(const GfxObject& handle) = 0;
    virtual void* MapIndexBuffer(const GfxObject& handle) = 0;
    virtual void UnmapVertexBuffer(const GfxObject& handle, const uint32_t count) = 0;
    virtual void UnmapIndexBuffer(const GfxObject& handle, const uint32_t count) = 0;

    virtual void DestroyVertexBuffer(const GfxObject& handle) = 0;
    virtual void DestroyIndexBuffer(const GfxObject& handle) = 0;
    virtual void DestroyTexture(const GfxObject& handle) = 0;
    virtual void DestroyPipelineState(const GfxObject& handle) = 0;
    virtual void DestroySamplerState(const GfxObject& hanele) = 0;

    // Actual rendering commands that operate on updated and ready resources.
    virtual void Render(const std::vector<RenderCommand>& commandBuffer) = 0;
};

}
