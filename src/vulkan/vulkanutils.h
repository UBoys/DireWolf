#pragma once

#include "direwolf/vulkan/vulkancommons.h"

#include <iostream> // TODO: Remove when moving pretty print
#include <vector>

namespace dw::vulkan {

VulkanRTLPtr GetRuntimeLibs();
bool InitProcAddress(VulkanRTLPtr vulkanRTL);
bool InitGlobalLevelFunction();

bool LoadInstanceLevelFunctions(const VkInstance& instance);
bool LoadInstanceLevelFunctionsFromExtensions(const VkInstance& instance, const std::vector<const char*>& enabledExtensions);
bool IsExtensionSupportedImpl(const char* extension, const std::vector<VkExtensionProperties>& availableExtensions);
bool LoadDeviceLevelFunctions(const VkDevice& device);
bool LoadDeviceLevelFunctionsFromExtensions(const VkDevice& logicalDevice, const std::vector<char const *>& enabledExtensions);
void FreeRuntimeLibrary(VulkanRTLPtr& vulkanRTL);

// TODO: prettyPrint should be moved to some other util, perhaps logUtils (remember the .inl file)
template <typename T>
void prettyPrint(std::ostream& os, const T& t, uint8_t minWidth = 0, const char* prefix = "", const char* suffix = "");

//std::ostream& operator<<(std::ostream& os, prettyPrint<>())

} // namespace dw::vulkan

#include "vulkanutils.inl"
