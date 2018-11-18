#include "direwolf/renderengine.h"

#if defined (DW_VULKAN_ENABLED)
#include "direwolf/vulkan/vulkansetup.h"
#include "vulkan/vulkanutils.h" // this is just for the log stuff.. TODO: put the log stuff in its own header
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
        std::cout << "\n" << device << " info: \n";
        std::vector<VkExtensionProperties> deviceExtensions = GetPhysicalDeviceExtensions(device);
        std::vector<VkQueueFamilyProperties> queueFamilies = GetQueueProperties(device);

        // We should create a bitset VkQueueFlags which holds info about which features we require from a queueFamily.
        // Available bits are: VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT and VK_QUEUE_SPARSE_BINDING_BIT
        const VkQueueFlags allFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT;
        const VkQueueFlags gfxAndComputeFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
        uint32_t allIndex;
        uint32_t gfxAndComputeIndex;
        bool isGfxAndComputePipelineSupported = GetSupportingQueueIndex(queueFamilies, gfxAndComputeFlags, gfxAndComputeIndex);
        bool isEverythingSupported = GetSupportingQueueIndex(queueFamilies, allFlags, allIndex);

        if (isEverythingSupported) {
            std::cout << "\t'VkQueueFamilyProperties' at index " << allIndex << " supports all functionalities." << std::endl;
        }

        if (isGfxAndComputePipelineSupported) {
            std::cout << "\t'VkQueueFamilyProperties' at index " << gfxAndComputeIndex << " supports all gfx and compute pipelines." << std::endl;
        }

        if (!isEverythingSupported && !isGfxAndComputePipelineSupported) {
            std::cerr << "WARNING: None of the VkQueueFamilyProperties has the required functionalites." << std::endl;
        }
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
