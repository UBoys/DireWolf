#include "direwolf/renderengine.h"

#include <iostream>

int main() {

    std::cout << "OpenGL example running\n";
    // Setup renderer
    // dw::InitData initData { dw::RendererType::RASTERIZER, dw::BackendType::VULKAN };
    // dw::PlatformData platformData = { nullptr };
    // auto engine = std::make_unique<dw::RenderEngine>(platformData, initData);

    std::cout << "\n\nPress enter to EXIT";
    std::cin.get();
    return 0;
}
