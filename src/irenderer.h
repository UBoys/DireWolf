#pragma once

#include <vector>

namespace dw {

enum RenderCommandType {
    BIND_PIPELINE_STATE,
    BIND_VERTEX_BUFFER,
    BIND_INDEX_BUFFER,
    BIND_TEXTURES,
    BIND_SAMPLERS,
    DRAW
};

// Handle for each renderer resource.
// All rendering resources are owned by the renderer and should not be coupled with client code
struct GfxObject {
    GfxObject() : id(0) {}
    explicit GfxObject(uint32_t id) : id(id) {}

    bool operator==(const GfxObject& rhs) const { return id == rhs.id; }
    bool operator<(const GfxObject& rhs) const { return id < rhs.id; }
    bool operator!=(const GfxObject& rhs) const { return id != rhs.id; }

    bool IsValid() const { return id != 0; }
    uint32_t id;
};

// Generic render command
struct RenderCommand {
    RenderCommandType type;
    void* data;
};

// Data associated with commands
struct BindVertexBufferCommandData {
    GfxObject* object;
};

struct BindConstantBufferCommandData {
    GfxObject* object;
};

struct BindPipelineStateCommandData {
    GfxObject* object;
};

struct DrawCommandData {
    uint32_t count;
    uint32_t startVertex;
    // TODO: Topology
};

struct RendererCaps {
    void* allocator;
    bool debug;
};

struct PlatformData;
struct InitData;
struct TextureDescription {};
struct SamplerDescription {};
// TODO: 
struct PipelineState {
	char* vertexShader;
	char* fragmentShader;
};

class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual void Initialize(const RendererCaps& caps, const PlatformData& platformData) = 0;

	virtual bool CreateConstantBuffer(const GfxObject& object, uint32_t count) = 0;
    virtual bool CreateVertexBuffer(const GfxObject& object, uint32_t count) = 0;
    virtual bool CreateIndexBuffer(const GfxObject& object, uint32_t count) = 0;
    virtual bool CreatePipelineState(const GfxObject& object, const PipelineState& state) = 0;
    virtual bool CreateTexture(const GfxObject& object, const TextureDescription& description, const std::vector<void*>& data) = 0;
    virtual bool CreateSamplerState(const GfxObject& object, const SamplerDescription& description) = 0;

	virtual void* MapConstantBuffer(const GfxObject& handle) = 0;
    virtual void* MapVertexBuffer(const GfxObject& handle) = 0;
    virtual void* MapIndexBuffer(const GfxObject& handle) = 0;
	virtual void UnmapConstantBuffer(const GfxObject& handle) = 0;
    virtual void UnmapVertexBuffer(const GfxObject& handle) = 0;
    virtual void UnmapIndexBuffer(const GfxObject& handle, uint32_t count) = 0;

    virtual void DestroyVertexBuffer(const GfxObject& handle) = 0;
    virtual void DestroyIndexBuffer(const GfxObject& handle) = 0;
    virtual void DestroyTexture(const GfxObject& handle) = 0;
    virtual void DestroyPipelineState(const GfxObject& handle) = 0;
    virtual void DestroySamplerState(const GfxObject& hanele) = 0;

    // Actual rendering commands that operate on updated and ready resources.
    virtual void Render(const std::vector<RenderCommand>& commandBuffer) = 0;
};

} // namespace dw
