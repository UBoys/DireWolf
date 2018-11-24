#pragma once

#include "vulkan/vulkan.h"

#include <iostream>
#include <string>

#if defined (_WIN32)
#include <windows.h>
#endif // _WIN32

namespace dw::vulkan {

#if defined (_WIN32)
typedef HMODULE VulkanRTLPtr;
#elif defined (__linux) || defined (__APPLE__)
typedef void* VulkanRTLPtr;
#else
typedef void* VulkanRTLPtr;
#endif

std::ostream& operator<<(std::ostream& os, const VkPhysicalDeviceType& deviceType);
std::ostream& operator<<(std::ostream& os, const VkPhysicalDevice& device);
std::ostream& operator<<(std::ostream& os, const VkExtensionProperties& properties);
std::ostream& operator<<(std::ostream& os, const VkPhysicalDeviceProperties& properties);

std::string toString(const VkPhysicalDeviceType& deviceType);
std::string toString(const VkPhysicalDevice& device);
std::string toString(const VkPhysicalDeviceProperties& properties);

} // namespace dw::vulkan
