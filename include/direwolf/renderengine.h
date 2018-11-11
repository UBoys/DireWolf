#pragma once

#include <memory>
#include "irenderer.h"
#include "common/config.h"

namespace dwf {

class RenderEngine {
public:
    RenderEngine(const PlatformData& platformData, const InitData& initData);
private:
    void _SetupRasterizer(const PlatformData& platformData, const BackendType& type);
    std::unique_ptr<IRenderer> m_renderer;
};

}  // namespace dwf
