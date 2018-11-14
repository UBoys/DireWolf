#include "direwolf/renderengine.h"

#include <iostream>
// TODO: Build definitions should decide what we include and build
#include "opengl/renderer_ogl.h"

namespace dw {

RenderEngine::RenderEngine(const PlatformData& platformData, const InitData& initData){
    switch(initData.rendererType) {
        case RASTERIZER:
            _SetupRasterizer(platformData, initData.backendType);
            break;
        case RAYTRACER:
            std::cerr << "Not implemented yet\n";
            break;
        default:
            std::cerr << "Unknown renderer type\n";
    }
}

bool RenderEngine::CreateVertexBuffer(const GfxObject& object, uint32_t count) const {
	return m_renderer->CreateVertexBuffer(object, count);
}

void* RenderEngine::MapVertexBuffer(const GfxObject& object) {
	return m_renderer->MapVertexBuffer(object);
}

void RenderEngine::UnmapVertexBuffer(const GfxObject& object) {
    m_renderer->UnmapVertexBuffer(object);
}

bool RenderEngine::CreatePipelineState(const GfxObject& object, const PipelineState& pipelineState) const {
	return m_renderer->CreatePipelineState(object, pipelineState);
}

void RenderEngine::Render(const std::vector<RenderCommand>& commandBuffer) const {
	return m_renderer->Render(commandBuffer);
}

void RenderEngine::_SetupRasterizer(const PlatformData& platformData, const BackendType& type) {
    std::cout << "DireWolf: Initializing render library!\n";
    RendererCaps caps = {};
    switch (type) {
        case OPENGL:
            m_renderer = std::make_unique<RendererOGL>();
            m_renderer->Initialize(caps, platformData);
            break;
        case VULKAN:
            // TODO: Callbam
            std::cerr << "No vulkan renderer implemented yet!\n";
            break;
        default:
            std::cerr << "What are you doing\n";
    }
}

}  // namespace dw
