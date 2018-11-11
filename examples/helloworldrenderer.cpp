#include "direwolf/renderengine.h"

#ifdef DW_VULKAN_ENABLED
#include "direwolf/vulkan/vulkansetup.h"
#endif // DW_VULKAN_ENABLED

#include <iostream>

void testNonEngineIntergratedVulkan()
{
    std::vector<const char*> desiredVulkanExtensions = { "VK_KHR_surface" };

#ifdef DW_VULKAN_ENABLED
    page::vulkan::TempVulkanSetupObject vulkanInitObj(&desiredVulkanExtensions);

    if (vulkanInitObj.isValid()) {
        std::cout << "\nVulkan was successfully initialized\n\n";
    } else {
        std::cout << "ERROR: Vulkan initialization failed\n";
    }
#else
    std::cerr << "You're trying to run VULKAN features, but haven't enabled it. Set DW_VULKAN_ENABLED to true in CMAKE to enable it." << std::endl;
#endif // DW_VULKAN_ENABLED
}

int main() {
    // Setup renderer
    dwf::InitData initData { dwf::RendererType::RASTERIZER, dwf::BackendType::VULKAN };
    dwf::PlatformData platformData = { nullptr };
    auto engine = std::make_unique<dwf::RenderEngine>(platformData, initData);

    testNonEngineIntergratedVulkan();

    std::cout << "\n\nPress enter to EXIT";
    std::cin.get();
    return 0;
}
