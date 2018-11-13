#pragma once

namespace dw {

// TODO: Figure out if this interface makes sense for all graphic API's.
// Do we swap buffers on Vulkan etc?
class IRenderContext {
public:
    virtual ~IRenderContext() = default;
    virtual void SwapBuffers() const = 0;
};

}