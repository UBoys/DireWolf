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

std::vector<VkQueueFamilyProperties> GetQueueProperties(const VkPhysicalDevice& device)
{
    uint32_t numQueueFamilies = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &numQueueFamilies, nullptr);
    if (numQueueFamilies == 0) {
        std::cerr << "ERROR: Couldn't get number of queue families!" << std::endl;
        return std::vector<VkQueueFamilyProperties>();
    }

    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    queueFamilyProperties.resize(numQueueFamilies);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &numQueueFamilies, queueFamilyProperties.data());
    if (numQueueFamilies == 0) {
        std::cerr << "ERROR: Couldn't get properties of the queue families!" << std::endl;
        return std::vector<VkQueueFamilyProperties>();
    }

    return queueFamilyProperties;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool IsQueueFamilySupportingFlags(const VkQueueFamilyProperties& queueFamily, const VkQueueFlags flagsToCheck)
{
    if (queueFamily.queueCount > 0 && (queueFamily.queueFlags & flagsToCheck) == flagsToCheck) {
        return true;
    }
    return false;
}

/** Finds first element in @param queueFamilies which fullfills all requirements in @param desiredFlags and sets @param outIndex to the corresponding index.
 *  @param outIndex remains unchanged if no queue fullfills the requirements
 *  returns false if no match is found.
 */
bool GetSupportingQueueIndex(const std::vector<VkQueueFamilyProperties>& queueFamilies, const VkQueueFlags desiredFlags, uint32_t& outIndex)
{
    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilies.size()); ++i) {
        if (IsQueueFamilySupportingFlags(queueFamilies[i], desiredFlags)) {
            outIndex = i;
            return true;
        }
    }

    return false;
}

VkDevice CreateLogicalDevice(const VkPhysicalDevice& physicalDevice, const VkDeviceCreateInfo& deviceCreateInfo)
{
    VkDevice logicalDevice;
    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice) != VK_SUCCESS || logicalDevice == VK_NULL_HANDLE) {
        std::cerr << "ERROR: Failed to create logical device" << std::endl;
        return VK_NULL_HANDLE;
    }

    // TODO: might not be nice to do this here........... move elsewhere?
    if (!LoadDeviceLevelFunctions(logicalDevice)) {
        std::cerr << "WARNING: Could load device level functions!" << std::endl;
    }

    return logicalDevice;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace dw::vulkan
