#include "direwolf/renderengine.h"

#include <iostream>

int main() {

    std::cout << "OpenGL example running\n";
    // Setup renderer
    // dwf::InitData initData { dwf::RendererType::RASTERIZER, dwf::BackendType::VULKAN };
    // dwf::PlatformData platformData = { nullptr };
    // auto engine = std::make_unique<dwf::RenderEngine>(platformData, initData);

    std::cout << "\n\nPress enter to EXIT";
    std::cin.get();
    return 0;
}
