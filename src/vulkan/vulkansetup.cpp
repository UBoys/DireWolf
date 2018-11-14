#include "direwolf/vulkan/vulkansetup.h"

#include "vulkanutils.h"

#include <iostream>
#include <string>

namespace dw::vulkan {

////////////////////////////////////////////////////////////////////////////////////////////////////

bool InitializeVulkan()
{
    // Get run-time library and initialize global level functions
    VulkanRTLPtr vulkanRTL = GetRuntimeLibs();
    if (!vulkanRTL || !InitProcAddress(vulkanRTL) || !InitGlobalLevelFunction()) {
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

VkInstance CreateVulkanInstance(const VulkanInstanceInitData& initData)
{
    // Get extension vector if user didn't already extract it by himself
    // TODO: We should look into getting a "scratch pad" arena for temporary variables like these.. what's the benefit?
    std::vector<VkExtensionProperties> availableExtensions;
    std::vector<VkExtensionProperties>* availableExtensionsHandle = initData.availableExtensionsHandle;
    if (!availableExtensionsHandle) {
        availableExtensions = GetAvailableInstanceExtensions();
        availableExtensionsHandle = &availableExtensions;
    }

    VkApplicationInfo applicationInfo;
    // The following could be set in an initializer list, but this is clearer for now
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = nullptr;
    applicationInfo.pApplicationName = initData.applicationName;
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = initData.engineName;
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo instanceCreateInfo;
    // The following could be set in an initializer list, but this is clearer for now
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext = nullptr;
    instanceCreateInfo.flags = 0;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledLayerCount = 0;
    instanceCreateInfo.ppEnabledLayerNames = nullptr;
    const std::vector<const char*>* desiredExtensions = initData.desiredExtensions;
    instanceCreateInfo.enabledExtensionCount = desiredExtensions ? desiredExtensions->size() : 0;
    instanceCreateInfo.ppEnabledExtensionNames = desiredExtensions ? desiredExtensions->data() : nullptr;

    VkInstance instance = VK_NULL_HANDLE;

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
        std::cerr << "Could not create Vulkan Instance." << std::endl;
        return VK_NULL_HANDLE;
    }

    if (!LoadInstanceLevelFunctions(instance) || (desiredExtensions && !LoadInstanceLevelFunctionsFromExtensions(instance, *desiredExtensions))) {
        return VK_NULL_HANDLE;
    }

    return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<VkExtensionProperties> GetAvailableInstanceExtensions()
{
    // Get number of available extensions
    uint32_t numExtensions = 0;
    if (vkEnumerateInstanceExtensionProperties(nullptr, &numExtensions, nullptr) != VK_SUCCESS || numExtensions == 0) {
        std::cerr << "Could not get the number of Vulkan Instance extensions." << std::endl;
        return std::vector<VkExtensionProperties>();
    }

    std::vector<VkExtensionProperties> returnVec;
    returnVec.resize(numExtensions);
    if (vkEnumerateInstanceExtensionProperties(nullptr, &numExtensions, returnVec.data()) != VK_SUCCESS || numExtensions == 0) {
        std::cerr << "Could not retrieve Vulkan Instance extensions." << std::endl;
        return std::vector<VkExtensionProperties>();
    }

#if defined( DW_VERBOSE_LOG_VK )
    std::cout << "\nThe following vulkan instance extensions are available:";
    std::cout << std::left << std::setfill(' ');
    for (const VkExtensionProperties& extensionProperty : returnVec) {
        prettyPrint(std::cout, extensionProperty, 40, "\n\t", "");
    }
    std::cout << std::endl;
#endif

    return returnVec;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<VkPhysicalDevice> GetPhysicalDevices(const VkInstance& instance)
{
    uint32_t numDevices = 0;
    if (vkEnumeratePhysicalDevices(instance, &numDevices, nullptr) != VK_SUCCESS || numDevices == 0) {
        std::cerr << "Could not get the number of available physical devices." << std::endl;
        return std::vector<VkPhysicalDevice>();
    }

    std::vector<VkPhysicalDevice> returnVec;
    returnVec.resize(numDevices);
    if (vkEnumeratePhysicalDevices(instance, &numDevices, returnVec.data()) != VK_SUCCESS) {
        std::cerr << "Could not enumerate physical devices." << std::endl;
        return std::vector<VkPhysicalDevice>();
    }

#if defined( DW_VERBOSE_LOG_VK )
    std::cout << "\nFound " << numDevices << " physical device(s):";
    std::cout << std::left << std::setfill(' ');
    for (const VkPhysicalDevice& physicalDevice : returnVec) {
        prettyPrint(std::cout, physicalDevice, 30, "\n\t", "");
    }
    std::cout << std::endl;
#endif

    return returnVec;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<VkExtensionProperties> GetPhysicalDeviceExtensions(const VkPhysicalDevice& device)
{

    uint32_t numExtensions = 0;
    if (vkEnumerateDeviceExtensionProperties(device, nullptr, &numExtensions, nullptr) != VK_SUCCESS || numExtensions == 0) {
        std::cerr << "Could not get the number of physical device extensions." << std::endl;
        return std::vector<VkExtensionProperties>();
    }

    std::vector<VkExtensionProperties> returnVec;
    returnVec.resize(numExtensions);
    if (vkEnumerateDeviceExtensionProperties(device, nullptr, &numExtensions, returnVec.data()) != VK_SUCCESS || numExtensions == 0) {
        std::cerr << "Could not enumerate physical device extensions." << std::endl;
        return std::vector<VkExtensionProperties>();
    }

#if defined( DW_VERBOSE_LOG_VK )
    std::cout << std::left << std::setfill(' ');
    std::cout << "\nThere are " << numExtensions << " extension properties for \"" << device << "\"";
    for (const VkExtensionProperties& extensionProperties : returnVec) {
        prettyPrint(std::cout, extensionProperties, 40, "\n\t", "");
    }
    std::cout << std::endl;
#endif

    return returnVec;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(const VkPhysicalDevice& device)
{
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    return deviceFeatures;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

VkPhysicalDeviceProperties GetPhysicalDeviceProperties(const VkPhysicalDevice& device)
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    return deviceProperties;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool IsExtensionSupported(const char* extension, const std::vector<VkExtensionProperties>& availableExtensions)
{
    return IsExtensionSupportedImpl(extension, availableExtensions);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace dw::vulkan
