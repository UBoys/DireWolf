#pragma once

#include <memory>
#include "irenderer.h"
#include "common/config.h"

namespace dw {

class RenderEngine {
public:
    RenderEngine(const PlatformData& platformData, const InitData& initData);

	bool CreateVertexBuffer(const GfxObject& object, uint32_t count) const;
	bool CreatePipelineState(const GfxObject& object, const PipelineState& pipelineState) const;
	void Render(const std::vector<RenderCommand>& commandBuffer) const;

private:
    void _SetupRasterizer(const PlatformData& platformData, const BackendType& type);
    std::unique_ptr<IRenderer> m_renderer;
};

}  // namespace dw
