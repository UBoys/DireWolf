#include <iostream>
#include "common/config.h"
#include "renderer_ogl.h"

#if defined(_WIN32)
#include "platform/rendercontext_ogl_win.h"
#endif

namespace dw {

void RendererOGL::Initialize(const RendererCaps& caps, const PlatformData& platformData) {
    std::cout << "DireWolf: Initializing OpenGL Renderer\n";

#if defined(_WIN32)
	m_renderContext = std::make_unique<RenderContextWin>(platformData);
#endif

	// Initialize context
}

}  // namespace dw
