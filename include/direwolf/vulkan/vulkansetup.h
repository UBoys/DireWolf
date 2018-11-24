/* **********************************************************************
Intended usage:

 1. Initialize vulkan
        InitializeVulkan()      [will open runtime libraries and load global level functions]

 2. Create vulkan instance object with preferences
    a)  [optional] See which instance extensions that are available by calling
            GetAvailableInstanceExtensions()

    b)  [optional] Create a vector<const char*> with the names of desired instance extensions
            Ensure these desired extensions exists in the vector returned by
            GetAvailableInstanceExtensions by calling IsExtensionSupported()

    c)  Create a VulkanInstanceData object and modify it as you please (Include data from 2a and 2b if you please)

    d)  Create the vulkan instance using the VulkanInstanceData you setup in step 2c) by calling
        CreateVulkanInstance(VulkanInstanceData)

 5. Choose which physical device to use
    a)  Get available physical devices
            GetPhysicalDevices()

    b)  See what extensions that are available on each devices
            GetPhysicalDeviceExtensions()

    c)  TODO: Pick the one best suited and do much more stuff afterwards ;)

    ... TODO:

********************************************************************** */

#pragma once

#if !defined(_WIN32) && !defined(__APPLE__) && !defined(__linux)
#error "The DireWolf renderer is not yet setup for Vulkan on this platform. Supported operating systems are Linux, Windows and MacOS";
#endif

#include <vector>

#include "vulkancommons.h"

namespace dw::vulkan {

struct VulkanInstanceInitData {
    char* applicationName = "NullNameApplication";
    char* engineName = "NullNameEngine";
    std::vector<VkExtensionProperties>* availableExtensionsHandle = nullptr;
    std::vector<const char*>* desiredExtensions = nullptr;
    // TODO: Extend this with more options?
};

struct QueueInfo {
    uint32_t familyIndex;
    std::vector<float> priorities;
};


VulkanRTLPtr InitializeVulkan();
VkInstance CreateVulkanInstance(const VulkanInstanceInitData& initData);
std::vector<VkExtensionProperties> GetAvailableInstanceExtensions();
std::vector<VkPhysicalDevice> GetPhysicalDevices(const VkInstance& instance);
std::vector<VkExtensionProperties> GetPhysicalDeviceExtensions(const VkPhysicalDevice& device);
VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(const VkPhysicalDevice& device);
VkPhysicalDeviceProperties GetPhysicalDeviceProperties(const VkPhysicalDevice& device);
std::vector<VkQueueFamilyProperties> GetQueueProperties(const VkPhysicalDevice& device);
bool GetSupportingQueueIndex(const std::vector<VkQueueFamilyProperties>& queueFamilies, const VkQueueFlags desiredFlag, uint32_t& outIndex);
bool IsQueueFamilySupportingFlags(const VkQueueFamilyProperties& queueFamily, const VkQueueFlags flagsToCheck);
bool IsExtensionSupported(const char* extension, const std::vector<VkExtensionProperties>& availableExtensions);
VkDevice CreateLogicalDevice(const VkPhysicalDevice& physicalDevice, const VkDeviceCreateInfo& deviceCreateInfo);
void DestroyLogicalDevice(VkDevice& logicalDevice);
void DestroyInstance(VkInstance& instance);
void ReleaseRuntimeLibrary(VulkanRTLPtr& vulkanRTL);

} // namespace dw::vulkan
