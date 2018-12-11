#pragma once

#include <memory>
#include "irenderer.h"
#include "common/config.h"

namespace dw {

class RenderEngine {
public:
    RenderEngine(const PlatformData& platformData, const InitData& initData);
    ~RenderEngine();

    bool CreateConstantBuffer(const GfxObject& object, uint32_t size) const;
    void* MapConstantBuffer(const GfxObject& object) const;
    void UnmapConstantBuffer(const GfxObject& object) const;

	bool CreateTexture(const GfxObject& object, const TextureDescription& description, void* data, uint32_t dataLength);
	void DestroyTexture(const GfxObject& object) const;

    /* Creates a vertex buffer resource */
    bool CreateVertexBuffer(const GfxObject& object, uint32_t count) const;
    /* Get a vertex buffer to fill with data */
    void* MapVertexBuffer(const GfxObject& object);
    /* Give back the data to the renderer that uploads it to the GPU */
    void UnmapVertexBuffer(const GfxObject& object);

    /* Creates a pipeline state resource - should contain shader, blendstate, depth state, rasterizer state */
    bool CreatePipelineState(const GfxObject& object, const PipelineState& pipelineState) const;
    /* Dispatch the rendering commands that covers one frame*/
    void Render(const std::vector<RenderCommand>& commandBuffer) const;

private:
    void _SetupRasterizer(const PlatformData& platformData, const BackendType& type);
    std::unique_ptr<IRenderer> m_renderer;
};

}  // namespace dw
