#pragma once

namespace dwf {

enum BackendType {
    NONE,
    OPENGL,
    VULKAN,
    D3D11,
    D3D12
};

enum RendererType {
    RASTERIZER,
    RAYTRACER
};

struct PlatformData {
    void* windowHandle = nullptr;
};

struct InitData {
    RendererType rendererType;
    BackendType backendType;
};

} // namespace pgfx
