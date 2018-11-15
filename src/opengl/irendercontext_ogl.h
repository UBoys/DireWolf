#pragma once

namespace dw {

class IRenderContextOGL {
public:
    virtual ~IRenderContextOGL() = default;
    virtual void SwapBuffers() const = 0;
};

}
