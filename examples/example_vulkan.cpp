#include "direwolf/renderengine.h"

#if defined (DW_VULKAN_ENABLED)
#include "direwolf/vulkan/vulkansetup.h"
#endif

#include <iostream>

void testNonEngineIntergratedVulkan()
{
#if defined (DW_VULKAN_ENABLED)
    std::cout << "\n\nTrying to use the non-integrated vulkan stuff\n";

    using namespace dw::vulkan;

    if (InitializeVulkan()) {
        std::cout << "\tVulkan was successfully initialized" << std::endl;
    } else {
        std::cerr << "ERROR: Vulkan initialization failed!" << std::endl;
        return;
    }

    // OPTIONAL: We can extract available instance extensions and verify that all extensions we want exist
    std::vector<VkExtensionProperties> availableExtensions = GetAvailableInstanceExtensions();
    std::vector<const char*> desiredVulkanExtensions = { "VK_KHR_surface" };
    bool areExtensionsSupported = true;
    for (const char* desiredExtension : desiredVulkanExtensions) {
        areExtensionsSupported &= IsExtensionSupported(desiredExtension, availableExtensions);
        // NOTE: If the extension isn't supported it'd have to be removed here - if not, the vulkan instance creation will fail!
    }

    if (!areExtensionsSupported) {
        std::cerr << "ERROR: Some of the required instance extensions are not supported!" << std::endl;
        return;
    }

    // We should create an VulkanInstanceData object.. we don't have to modify any of the default data but names will default to "NullName<*>" if we don't
    VulkanInstanceInitData vulkanInitData;
    vulkanInitData.applicationName = "DireWolf Vulkan Application";
    vulkanInitData.engineName = "DireWolf Engine";
    vulkanInitData.availableExtensionsHandle = &availableExtensions; // This is optional, it can be left as nullptr. The object will then find this vector by itself
    vulkanInitData.desiredExtensions = &desiredVulkanExtensions; // This is optional, it can be left as nullptr.

    VkInstance vulkanInstance = CreateVulkanInstance(vulkanInitData);

    std::vector<VkPhysicalDevice> physicalDevices = GetPhysicalDevices(vulkanInstance);

    for (const VkPhysicalDevice& device : physicalDevices) {
        std::vector<VkExtensionProperties> deviceExtensions = GetPhysicalDeviceExtensions(device);
    }
#else
    std::cerr << "You're trying to run VULKAN features, but haven't enabled it. Set DW_VULKAN_ENABLED to true in CMAKE to enable it." << std::endl;
#endif // DW_VULKAN_ENABLED
}

int main() {
    // Setup renderer
    // dw::InitData initData { dw::RendererType::RASTERIZER, dw::BackendType::VULKAN };
    // dw::PlatformData platformData = { nullptr };
    // auto engine = std::make_unique<dw::RenderEngine>(platformData, initData);

    testNonEngineIntergratedVulkan();

    std::cout << "\n\nPress enter to EXIT";
    std::cin.get();
    return 0;
}
