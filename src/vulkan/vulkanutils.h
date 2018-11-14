#pragma once

#include "vulkan/vulkanfunctions.h"

#include <iostream>
#include <string>
#include <vector>

#if defined _WIN32
#include <windows.h>
#endif // _WIN32

namespace dw::vulkan {

// Define the VulkanRTLPtr type based on platform
#if defined (_WIN32)
typedef HMODULE VulkanRTLPtr;
#elif defined (__linux) || defined (__APPLE__)
typedef void* VulkanRTLPtr;
#else
typedef void* VulkanRTLPtr;
#endif

VulkanRTLPtr GetRuntimeLibs();
bool InitProcAddress(VulkanRTLPtr vulkanRTL);
bool InitGlobalLevelFunction();

bool LoadInstanceLevelFunctions(const VkInstance& instance);
bool LoadInstanceLevelFunctionsFromExtensions(const VkInstance& instance, const std::vector<const char*>& enabledExtensions);
bool IsExtensionSupportedImpl(const char* extension, const std::vector<VkExtensionProperties>& availableExtensions);

// TODO: prettyPrint should be moved to some other util, perhaps logUtils (remember the .inl file)

template <typename T>
void prettyPrint(std::ostream& os, const T& t, uint8_t minWidth = 0, const char* prefix = "", const char* suffix = "");

//std::ostream& operator<<(std::ostream& os, prettyPrint<>())

std::ostream& operator<<(std::ostream& os, const VkPhysicalDeviceType& deviceType);
std::ostream& operator<<(std::ostream& os, const VkPhysicalDevice& device);
std::ostream& operator<<(std::ostream& os, const VkExtensionProperties& properties);
std::ostream& operator<<(std::ostream& os, const VkPhysicalDeviceProperties& properties);

std::string toString(const VkPhysicalDeviceType& deviceType);
std::string toString(const VkPhysicalDevice& device);
std::string toString(const VkPhysicalDeviceProperties& properties);

} // namespace dw::vulkan

#include "vulkanutils.inl"