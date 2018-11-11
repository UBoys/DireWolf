#include "direwolf/vulkan/vulkansetup.h"

#include "vulkanutils.h"

#include <cstring>
#include <iostream>
#include <string>

namespace page::vulkan {

////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: should probably take some init-struct
TempVulkanSetupObject::TempVulkanSetupObject(std::vector<const char*>* desiredExtensions /* = nullptr */)
: m_isValid(false)
, m_vulkanRTLFound(false)
, m_instance(VK_NULL_HANDLE)
{
#if defined( DW_VERBOSE_LOG_VK )
    std::cout << "\nTemporaryVulkanSetupObject ctor\n";
#endif
    m_isValid = initialize(desiredExtensions);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool TempVulkanSetupObject::initialize(std::vector<const char*>* desiredExtensions)
{
    if (m_isValid) {
        std::cerr << "Trying to initialize TempVulkanSetupObject after it has already been initialized!" << std::endl;
        return false;
    }

    // TODO: this is just temp logic..
    {
        std::vector<VkPhysicalDevice> physicalDevices;
        // Out as soon as something goes wrong
        if (!initLibs() ||
            !initProcAddr() ||
            !loadGlobalLevelFunctions() ||
            !createVulkanInstance(desiredExtensions) ||
            !loadInstanceLevelFunctions() ||
            !loadInstanceLevelFunctionsFromExtensions() ||
            !getPhysicalDevices(physicalDevices))
        {
            return false;
        }

        for (const VkPhysicalDevice& physicalDevice : physicalDevices) {
            std::vector<VkExtensionProperties> extensionProperties;
            getPhysicalDeviceExtensions(physicalDevice, extensionProperties);

            VkPhysicalDeviceFeatures deviceFeatures;
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
            vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool TempVulkanSetupObject::initLibs()
{
#if defined( _WIN32 )
    vulkan_library = LoadLibrary("vulkan-1.dll");
#elif defined( __linux )
    vulkan_library = dlopen("libvulkan.so.1", RTLD_NOW);
#else
    std::cerr << "The PAGE renderer is not yet setup for Vulkan on this platform. Supported operating systems are Linux and Windows" << std::endl;
    vulkan_library = nullptr;
#endif
    if (!vulkan_library) {
        std::cerr << "Could not connect with a Vulkan Runtime library.\n";
        m_vulkanRTLFound = false;
        return false;
    }

#if defined ( DW_VERBOSE_LOG_VK )
    std::cout << "\tSuccessfully connected with a Vulkan Runtime library.\n";
#endif

    m_vulkanRTLFound = true;
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool TempVulkanSetupObject::initProcAddr()
{
    // TODO: move this #ifdef part elsewhere?
#if defined( DW_VERBOSE_LOG_VK )
#if defined( VK_NO_PROTOTYPES )
    std::cout << "\tVK_NO_PROTOTYPES is defined\n";
#else // VK_NO_PROTOTYPES
    std::cout << "\tWARNING: VK_NO_PROTOTYPES is NOT defined. This may become a potential performance issue.\n";
#endif // VK_NO_PROTOTYPES
#endif // DW_VERBOSE_LOG_VK

#if defined( _WIN32 )
#define LoadFunction GetProcAddress
#elif defined __linux
#define LoadFunction dlsym
#endif

#define EXPORTED_VULKAN_FUNCTION( name )                              \
    name = (PFN_##name)LoadFunction( vulkan_library, #name );         \
    if ( name == nullptr ) {                                          \
        std::cerr << "Could not load exported Vulkan function named: "\
            #name << std::endl;                                       \
        return false;                                                 \
    }

#include "listofvulkanfunctions.inl"

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool TempVulkanSetupObject::loadGlobalLevelFunctions()
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

bool TempVulkanSetupObject::loadInstanceLevelFunctions()
{
#define INSTANCE_LEVEL_VULKAN_FUNCTION( name )                              \
    name = (PFN_##name)vkGetInstanceProcAddr( m_instance, #name );          \
    if ( name == nullptr ) {                                                \
        std::cerr << "Could not load instance-level Vulkan function named: "\
            #name << std::endl;                                             \
        return false;                                                       \
    }                                                                       \

#include "listofvulkanfunctions.inl"

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool TempVulkanSetupObject::loadInstanceLevelFunctionsFromExtensions(const std::vector<const char*>* enabledExtensions /* = nullptr */)
{
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension )                \
    if (enabledExtensions) {                                                            \
        for (const char* const & enabledExtension : *enabledExtensions) {               \
            if (std::string(enabledExtension) == std::string(extension)) {              \
                name = (PFN_##name)vkGetInstanceProcAddr(m_instance, #name);            \
                if( name == nullptr ) {                                                 \
                    std::cerr << "Could not load instance-level Vulkan function named: "\
                        #name << std::endl;                                             \
                    return false;                                                       \
                }                                                                       \
            }                                                                           \
        }                                                                               \
    }

#include "listofvulkanfunctions.inl"

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: this needs refactoring
bool TempVulkanSetupObject::getAvailableInstanceExtensions(std::vector<VkExtensionProperties>& outAvailableExtensions) const
{
    if (!m_vulkanRTLFound) {
        return false;
    }

    // Input should be an empty vector
    outAvailableExtensions.clear();

    uint32_t numExtensions = 0;

    // Get number of available extensions
    if (vkEnumerateInstanceExtensionProperties(nullptr, &numExtensions, nullptr) != VK_SUCCESS || numExtensions == 0) {
        std::cerr << "Could not get the number of Vulkan Instance extensions." << std::endl;
        return false;
    }

    outAvailableExtensions.resize(numExtensions);
    if (vkEnumerateInstanceExtensionProperties(nullptr, &numExtensions, outAvailableExtensions.data()) != VK_SUCCESS || outAvailableExtensions.size() == 0) {
        std::cerr << "Could not retrieve Vulkan Instance extensions." << std::endl;
        return false;
    }

#if defined( DW_VERBOSE_LOG_VK )
    std::cout << "\nThe following vulkan instance extensions are available:\n";
    std::cout << std::left << std::setfill(' ');
    for (const VkExtensionProperties& extensionProperty : outAvailableExtensions) {
        prettyPrint(std::cout, extensionProperty, 40, "\t", "\n");
    }
    std::cout << std::endl;
#endif

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool TempVulkanSetupObject::createVulkanInstance(std::vector<const char*>* desiredExtensions)
{
    std::vector<VkExtensionProperties> availableExtensions;
    if (!getAvailableInstanceExtensions(availableExtensions)) {
        return false;
    }

    if (desiredExtensions) {
        for (const char* extension : *desiredExtensions) {
            if (!isExtensionSupported(extension, &availableExtensions)) {
                std::cerr << "Extension named '" << extension << "' is not supported." << std::endl;
                return false;
            }
        }
    }

    VkApplicationInfo applicationInfo;
    // The following could be set in an initializer list, but this is clearer for now
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = nullptr;
    applicationInfo.pApplicationName = "NullNameApplication";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = "NullNameEngine";
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
    instanceCreateInfo.enabledExtensionCount = desiredExtensions ? desiredExtensions->size() : 0;
    instanceCreateInfo.ppEnabledExtensionNames = desiredExtensions ? desiredExtensions->data() : nullptr;

    if ( (vkCreateInstance( &instanceCreateInfo, nullptr, &m_instance ) != VK_SUCCESS) || (m_instance == VK_NULL_HANDLE) ) {
        std::cerr << "Could not create Vulkan Instance." << std::endl;
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<const char*> TempVulkanSetupObject::getDefaultInstanceExtensions() {
    return {
        "VK_KHR_surface",
    // "VK_KHR_win32_surface",
    // "VK_KHR_external_memory_capabilities",
    // "VK_KHR_external_semaphore_capabilities",
    // "VK_KHR_external_fence_capabilities",
    // "VK_KHR_get_physical_device_properties2",
    // "VK_KHR_get_surface_capabilities2",
    // "VK_EXT_debug_report",
    // "VK_EXT_display_surface_counter",
    // "VK_NV_external_memory_capabilities",
    // "VK_EXT_debug_utils"
    };
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool TempVulkanSetupObject::isExtensionSupported(const char* extension, std::vector<VkExtensionProperties>* availableExtensionsPtr /* = nullptr*/) const
{
    std::vector<VkExtensionProperties> availableExtensions;
    if (!availableExtensionsPtr) {
        getAvailableInstanceExtensions(availableExtensions);
        availableExtensionsPtr = &availableExtensions;
    }

    for (VkExtensionProperties availableExtension : *availableExtensionsPtr) {
        if (strcmp(availableExtension.extensionName, extension) == 0)
        {
            return true;
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool TempVulkanSetupObject::getPhysicalDevices(std::vector<VkPhysicalDevice>& outAvailableDevices)
{
    uint32_t numDevices = 0;
    if (vkEnumeratePhysicalDevices(m_instance, &numDevices, nullptr) != VK_SUCCESS || numDevices == 0) {
        std::cerr << "Could not get the number of available physical devices." << std::endl;
        return false;
    }

    outAvailableDevices.resize(numDevices);
    if (vkEnumeratePhysicalDevices(m_instance, &numDevices, outAvailableDevices.data()) != VK_SUCCESS) {
        std::cerr << "Could not enumerate physical devices." << std::endl;
        return false;
    }

#if defined( DW_VERBOSE_LOG_VK )
    std::cout << "\nFound " << numDevices << " physical device(s):\n";
    std::cout << std::left << std::setfill(' ');
    for (const VkPhysicalDevice& device : outAvailableDevices) {
        prettyPrint(std::cout, device, 30, "\t", "\n");
    }
#endif

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool TempVulkanSetupObject::getPhysicalDeviceExtensions(const VkPhysicalDevice& device, std::vector<VkExtensionProperties>& outAvailableExtensions)
{
    uint32_t numExtensions = 0;
    if (vkEnumerateDeviceExtensionProperties(device, nullptr, &numExtensions, nullptr) != VK_SUCCESS || numExtensions == 0) {
        std::cerr << "Could not get the number of physical device extensions." << std::endl;
        return false;
    }

    outAvailableExtensions.resize(numExtensions);
    if (vkEnumerateDeviceExtensionProperties(device, nullptr, &numExtensions, outAvailableExtensions.data()) != VK_SUCCESS || numExtensions == 0) {
        std::cerr << "Could not enumerate physical device extensions." << std::endl;
        return false;
    }

#if defined( DW_VERBOSE_LOG_VK )
    std::cout << std::left << std::setfill(' ');
    std::cout << "\nThere are " << numExtensions << " extension properties for \"" << device << "\"\n";
    for (const VkExtensionProperties& extensionProperties : outAvailableExtensions) {
        prettyPrint(std::cout, extensionProperties, 40, "\t", "\n");
    }
#endif

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace page::vulkan
