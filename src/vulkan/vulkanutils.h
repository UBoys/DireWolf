#pragma once

#include "vulkan/vulkanfunctions.h"

#include <iostream>
#include <string>

namespace page::vulkan {

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

} // namespace page::vulkan

#include "vulkanutils.inl"