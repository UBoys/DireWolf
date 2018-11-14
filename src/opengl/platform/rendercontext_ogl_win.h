#pragma once

#include <windows.h>
#include "opengl/irendercontext_ogl.h"

namespace dw {

struct PlatformData;

class RenderContextWin final : public IRenderContextOGL {
public:
    RenderContextWin(const PlatformData& platformData);
    virtual void SwapBuffers() const override;
};

}  // namespace dw