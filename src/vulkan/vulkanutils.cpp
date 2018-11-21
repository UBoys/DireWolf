#include "vulkanutils.h"

#include "vulkan/vulkanfunctions.h"


#if defined(__APPLE__) || defined(__linux)
  #include <dlfcn.h>
#endif

namespace dw::vulkan {

////////////////////////////////////////////////////////////////////////////////////////////////////

VulkanRTLPtr GetRuntimeLibs()
{
    VulkanRTLPtr vulkanLibrary;

#if defined (_WIN32)
    vulkanLibrary = LoadLibrary("vulkan-1.dll");
#elif defined (__linux )
    vulkanLibrary = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
#elif defined (__APPLE__ )
    vulkanLibrary = dlopen("libvulkan.1.dylib", RTLD_NOW | RTLD_LOCAL);
#else
    std::cerr << "The DireWolf renderer is not yet setup for Vulkan on this platform. Supported operating systems are Linux, Windows and Mac" << std::endl;
    vulkanLibrary = nullptr;
#endif

    if (!vulkanLibrary) {
        std::cerr << "Could not connect with a Vulkan Runtime library.\n";
    }

#if defined (DW_VERBOSE_LOG_VK)
    std::cout << "\tSuccessfully connected with a Vulkan Runtime library.\n";
#endif
    return vulkanLibrary;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool InitProcAddress(VulkanRTLPtr vulkanRTL)
{
// TODO: move this #VK_NO_PROTOTYPES part elsewhere?
#if defined (DW_VERBOSE_LOG_VK) && defined (VK_NO_PROTOTYPES)
    std::cout << "\tVK_NO_PROTOTYPES is defined\n";
#elif defined (DW_VERBOSE_LOG_VK) && !defined (VK_NO_PROTOTYPES)
    std::cout << "\tWARNING: VK_NO_PROTOTYPES is NOT defined. This may become a potential performance issue.\n";
#endif

#if defined (_WIN32)
  #define LoadFunction GetProcAddress
#elif defined (__linux)
  #define LoadFunction dlsym
#elif defined (__APPLE__)
  #define LoadFunction dlsym
#endif

#define EXPORTED_VULKAN_FUNCTION( name )                              \
    name = (PFN_##name)LoadFunction( vulkanRTL, #name );              \
    if ( name == nullptr ) {                                          \
        std::cerr << "Could not load exported Vulkan function named: "\
            #name << std::endl;                                       \
        return false;                                                 \
    }

#include "listofvulkanfunctions.inl"

    return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////

bool InitGlobalLevelFunction()
{
#define GLOBAL_LEVEL_VULKAN_FUNCTION( name )                        \
    name = (PFN_##name)vkGetInstanceProcAddr( nullptr, #name );     \
    if ( name == nullptr ) {                                        \
        std::cerr << "Could not load global-level function named: " \
            #name << std::endl;                                     \
        return false;                                               \
    }

#include "listofvulkanfunctions.inl"

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool LoadInstanceLevelFunctions(const VkInstance& instance)
{
#define INSTANCE_LEVEL_VULKAN_FUNCTION( name )                              \
    name = (PFN_##name)vkGetInstanceProcAddr( instance, #name );            \
    if ( name == nullptr ) {                                                \
        std::cerr << "Could not load instance-level Vulkan function named: "\
            #name << std::endl;                                             \
        return false;                                                       \
    }                                                                       \

#include "listofvulkanfunctions.inl"

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool LoadInstanceLevelFunctionsFromExtensions(const VkInstance& instance, const std::vector<const char*>& enabledExtensions)
{
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension )            \
    for (const char* enabledExtension : enabledExtensions) {                        \
        if (std::string(enabledExtension) == std::string(extension)) {              \
            name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);              \
            if( name == nullptr ) {                                                 \
                std::cerr << "Could not load instance-level Vulkan function named: "\
                    #name << std::endl;                                             \
                return false;                                                       \
            }                                                                       \
        }                                                                           \
    }

#include "listofvulkanfunctions.inl"

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool IsExtensionSupportedImpl(const char* extension, const std::vector<VkExtensionProperties>& availableExtensions)
{
    for (const VkExtensionProperties& availableExtension : availableExtensions) {
        if (strcmp(availableExtension.extensionName, extension) == 0) {
            return true;
        }
    }

    std::cerr << "WARNING: \"" << extension << "\" is not a valid extension!" << std::endl;

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool LoadDeviceLevelFunctions(const VkDevice& logicalDevice)
{
#define DEVICE_LEVEL_VULKAN_FUNCTION( name )                                                    \
    name = (PFN_##name)vkGetDeviceProcAddr( logicalDevice, #name );                             \
    if( name == nullptr ) {                                                                     \
        std::cout << "Could not load device-level Vulkan function named: " #name << std::endl;  \
        return false;                                                                           \
    }

#include "listofvulkanfunctions.inl"

    return true;
}

////////////////////////////////////////////////////////////////////////////////

// TODO: Not tested yet
bool LoadDeviceLevelFunctionsFromExtensions(const VkDevice& logicalDevice, const std::vector<char const *>& enabledExtensions)
{
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension )                                  \
    for (const char* enabledExtension : enabledExtensions) {                                            \
        if (std::string(enabledExtension) == std::string(extension)) {                                  \
            name = (PFN_##name)vkGetDeviceProcAddr(logicalDevice, #name);                               \
            if (name == nullptr) {                                                                      \
                std::cerr << "Could not load device-level Vulkan function named: " #name << std::endl;  \
                return false;                                                                           \
            }                                                                                           \
        }                                                                                               \
    }

#include "listofvulkanfunctions.inl"
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void FreeRuntimeLibrary(VulkanRTLPtr& vulkanRTL)
{
#if defined (_WIN32)
    FreeLibrary(vulkanRTL);
#elif defined (__linux) || defined (__APPLE__)
    dlclose(vulkanRTL);
#endif
    vulkanRTL = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace dw::vulkan
