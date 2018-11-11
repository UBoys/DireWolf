#include "vulkanutils.h"

#include <iomanip>
#include <sstream>

// TODO: fix namespaces
namespace page::vulkan {

////////////////////////////////////////////////////////////////////////////////////////////////////
// overloaded operator<<
std::ostream& operator<<(std::ostream& os, const VkPhysicalDeviceType& deviceType)
{
    switch (deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER          : os << "OTHER";          break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU : os << "INTEGRATED GPU"; break;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU   : os << "DISCRETE GPU";   break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU    : os << "VIRTUAL GPU";    break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU            : os << "CPU";            break;
        default                                     : os << "UNRECOGNISED";   break;
    }
    return os;
}
std::ostream& operator<<(std::ostream& os, const VkPhysicalDevice& device)
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);
    return os << properties;
}
std::ostream& operator<<(std::ostream& os, const VkExtensionProperties& properties)
{
    return os << properties.extensionName << " [ " << properties.specVersion << " ]";
}
std::ostream& operator<<(std::ostream& os, const VkPhysicalDeviceProperties& properties)
{
    return os << properties.deviceName << " [ " << properties.deviceType << " ]";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// overloaded toString functions
std::string toString(const VkPhysicalDeviceType& deviceType)
{
    std::ostringstream os;
    os << deviceType;
    return os.str();
}
std::string toString(const VkPhysicalDevice& device)
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);
    return toString(properties);
}
std::string toString(const VkPhysicalDeviceProperties& properties)
{
    std::ostringstream os;
    os << properties;
    return os.str();
}

} // namespace page::vulkan
