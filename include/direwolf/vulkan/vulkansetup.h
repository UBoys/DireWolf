#pragma once

#include "vulkan/vulkanfunctions.h"

#include <stdint.h>
#include <vector>

#if defined _WIN32
#include <windows.h>
#endif // _WIN32

namespace page {
namespace vulkan {

class TempVulkanSetupObject {
public:
    // TODO: add struct to hold initialize data.. e.g. desiredExtensions and names etc. See createVulkanInstance implementation to get an idea of what it should hold
    TempVulkanSetupObject(std::vector<const char*>* desiredExtensions = nullptr);
    bool initialize(std::vector<const char*>* desiredExtensions);
    bool isValid() const { return m_isValid; }
    static std::vector<const char*> getDefaultInstanceExtensions();

private: // variables
    bool m_isValid;
    bool m_vulkanRTLFound;
    VkInstance m_instance;

#if defined _WIN32
    HMODULE vulkan_library;
#elif defined __linux
    void* vulkan_library;
#else
    void* vulkan_library;
#endif

private: // functions
    bool initLibs();
    bool initProcAddr();
    bool isExtensionSupported(const char* extension, std::vector<VkExtensionProperties>* availableExtensions = nullptr) const;
    bool loadGlobalLevelFunctions();
    bool loadInstanceLevelFunctions();
    bool getPhysicalDevices(std::vector<VkPhysicalDevice>& outAvailableDevices);
    bool getPhysicalDeviceExtensions(const VkPhysicalDevice& device, std::vector<VkExtensionProperties>& outAvailableExtensions);
    bool loadInstanceLevelFunctionsFromExtensions(const std::vector<const char*>* enabledExtensions = nullptr);
    bool createVulkanInstance(std::vector<const char*>* desiredExtensions = nullptr);
    bool getAvailableInstanceExtensions(std::vector<VkExtensionProperties>& outAvailableExtensions) const;
};

} // namespace vulkan
} // namespace page
