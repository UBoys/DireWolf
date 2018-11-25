#pragma once

#include "opengl/irendercontext_ogl.h"

namespace dw {

struct PlatformData;

class RenderContextOSX final : public IRenderContextOGL {
public:
    RenderContextOSX(const PlatformData& platformData);

    virtual ~RenderContextOSX() override;
    virtual void SwapBuffers() const override;
};

}  // namespace dw
