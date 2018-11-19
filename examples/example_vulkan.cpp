#include "direwolf/renderengine.h"

#if defined (DW_VULKAN_ENABLED)
#include "direwolf/vulkan/vulkansetup.h"
#include "vulkan/vulkanutils.h" // this is just for the log stuff.. TODO: put the log stuff in its own header
#endif

#include <iostream>

void testNonEngineIntergratedVulkan()
{
#if defined (DW_VULKAN_ENABLED)
    std::cout << "\n\nTrying to use the non-integrated vulkan stuff\n";

    using namespace dw::vulkan;

    if (InitializeVulkan()) {
        std::cout << "\tVulkan was successfully initialized" << std::endl;
    } else {
        std::cerr << "ERROR: Vulkan initialization failed!" << std::endl;
        return;
    }

    // OPTIONAL: We can extract available instance extensions and verify that all extensions we want exist
    std::vector<VkExtensionProperties> availableExtensions = GetAvailableInstanceExtensions();
    std::vector<const char*> desiredVulkanExtensions = { "VK_KHR_surface" };
    bool areRequirementsMet = true;
    for (const char* desiredExtension : desiredVulkanExtensions) {
        areRequirementsMet &= IsExtensionSupported(desiredExtension, availableExtensions);
        // NOTE: If the extension isn't supported it'd have to be removed here - if not, the vulkan instance creation will fail!
    }

    if (!areRequirementsMet) {
        std::cerr << "ERROR: Some of the required instance extensions are not supported!" << std::endl;
        return;
    }

    // We should create an VulkanInstanceData object.. we don't have to modify any of the default data but names will default to "NullName<*>" if we don't
    VulkanInstanceInitData vulkanInitData;
    vulkanInitData.applicationName = "DireWolf Vulkan Application";
    vulkanInitData.engineName = "DireWolf Engine";
    vulkanInitData.availableExtensionsHandle = &availableExtensions; // This is optional, it can be left as nullptr. The object will then find this vector by itself
    vulkanInitData.desiredExtensions = &desiredVulkanExtensions; // This is optional, it can be left as nullptr.

    VkInstance vulkanInstance = CreateVulkanInstance(vulkanInitData);

    std::vector<VkPhysicalDevice> physicalDevices = GetPhysicalDevices(vulkanInstance);

    VkPhysicalDevice selectedDevice = VK_NULL_HANDLE;
    std::vector<QueueInfo> queueInfos; // TODO: should be a set?

    // TODO: Only temp.. These are all the extensions available on Dell XPS with GTX 1050.. using a subset of them as desiredExtensions now
    const std::vector<char const *> desiredDeviceExtensions{
        "VK_KHR_swapchain",
        //"VK_KHR_16bit_storage",
        //"VK_KHR_bind_memory2",
        //"VK_KHR_dedicated_allocation",
        //"VK_KHR_descriptor_update_template",
        //"VK_KHR_get_memory_requirements2",
        //"VK_KHR_image_format_list",
        //"VK_KHR_maintenance1",
        //"VK_KHR_maintenance2",
        //"VK_KHR_push_descriptor",
        //"VK_KHR_relaxed_block_layout",
        //"VK_KHR_sampler_mirror_clamp_to_edge",
        //"VK_KHR_sampler_ycbcr_conversion",
        //"VK_KHR_shader_draw_parameters",
        //"VK_KHR_storage_buffer_storage_class",
        //"VK_KHR_external_memory",
        //"VK_KHR_external_memory_win32",
        //"VK_KHR_external_semaphore",
        //"VK_KHR_external_semaphore_win32",
        //"VK_KHR_win32_keyed_mutex",
        //"VK_KHR_external_fence",
        //"VK_KHR_external_fence_win32",
        //"VK_KHR_variable_pointers",
        //"VK_KHX_device_group",
        //"VK_KHX_multiview",
        //"VK_EXT_blend_operation_advanced",
        //"VK_EXT_depth_range_unrestricted",
        //"VK_EXT_discard_rectangles",
        //"VK_EXT_post_depth_coverage",
        //"VK_EXT_sample_locations",
        //"VK_EXT_sampler_filter_minmax",
        //"VK_EXT_shader_subgroup_ballot",
        //"VK_EXT_shader_subgroup_vote",
        //"VK_EXT_shader_viewport_index_layer",
        //"VK_NV_dedicated_allocation",
        //"VK_NV_external_memory",
        //"VK_NV_external_memory_win32",
        //"VK_NV_fill_rectangle",
        //"VK_NV_fragment_coverage_to_color",
        //"VK_NV_framebuffer_mixed_samples",
        //"VK_NV_glsl_shader",
        //"VK_NV_win32_keyed_mutex",
        //"VK_NV_clip_space_w_scaling",
        //"VK_NV_sample_mask_override_coverage",
        //"VK_NV_viewport_array2",
        //"VK_NV_viewport_swizzle",
        //"VK_NV_geometry_shader_passthrough",
        //"VK_NVX_device_generated_commands",
        //"VK_NVX_multiview_per_view_attributes"
    };

    // TODO: this whole part is a mess.. needs to be redone
    for (const VkPhysicalDevice& device : physicalDevices) {
        areRequirementsMet = true;
        std::cout << "\n" << device << " info: \n";
        std::vector<VkExtensionProperties> deviceExtensions = GetPhysicalDeviceExtensions(device);


        for (const char* desiredExtension : desiredDeviceExtensions) {
            areRequirementsMet &= IsExtensionSupported(desiredExtension, deviceExtensions);
        }

        if (!areRequirementsMet) {
            std::cerr << "WARNING: Some of the required device extensions are not supported on " << device << "!" << std::endl;
            continue;
        }

        VkPhysicalDeviceFeatures deviceFeatures = GetPhysicalDeviceFeatures(device);

        areRequirementsMet &= deviceFeatures.vertexPipelineStoresAndAtomics && deviceFeatures.fragmentStoresAndAtomics;

        if (!areRequirementsMet) {
            std::cerr << "WARNING: Some of the required device features are not supported on " << device << "!" << std::endl;
            continue;
        }

        // TODO: Do something with the properties here?
        VkPhysicalDeviceProperties deviceProperties = GetPhysicalDeviceProperties(device);

        std::vector<VkQueueFamilyProperties> queueFamilies = GetQueueProperties(device);

        // We should create a bitset VkQueueFlags which holds info about which features we require from a queueFamily.
        // Available bits are: VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT and VK_QUEUE_SPARSE_BINDING_BIT
        const VkQueueFlags allFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT;
        uint32_t allIndex;
        const VkQueueFlags graphicsPipelineFlag = VK_QUEUE_GRAPHICS_BIT;
        uint32_t gfxIndex;
        bool isGfxPipelineSupported = GetSupportingQueueIndex(queueFamilies, graphicsPipelineFlag, gfxIndex);
        bool isEverythingSupported = GetSupportingQueueIndex(queueFamilies, allFlags, allIndex);
        // TODO: should probably not use GetSupportingQueueIndex.. IsQueueFamilySupportingFlags might be better to use

        // TODO: this following part makes no sense.. rewrite when we know how we want to deal with this
        if (isEverythingSupported) {
            std::cout << "\t'VkQueueFamilyProperties' at index " << allIndex << " supports all functionalities." << std::endl;
            queueInfos.push_back(QueueInfo({ gfxIndex }));
            for (uint32_t i = 0; i < queueFamilies[gfxIndex].queueCount; ++i) {
                queueInfos[0].priorities.push_back(1.0f);
            }
            selectedDevice = device;
            break;
        }

        if (isGfxPipelineSupported) {
            std::cout << "\t'VkQueueFamilyProperties' at index " << gfxIndex << " supports praphics pipelines." << std::endl;
            queueInfos.push_back(QueueInfo({ gfxIndex }));
            for (uint32_t i = 0; i < queueFamilies[gfxIndex].queueCount; ++i) {
                queueInfos[0].priorities.push_back(1.0f);
            }
            selectedDevice = device;
            break;
        }

        if (!isEverythingSupported && !isGfxPipelineSupported) {
            std::cerr << "WARNING: None of the VkQueueFamilyProperties has the required functionalites." << std::endl;
        }
    }

    if (selectedDevice == VK_NULL_HANDLE) {
        std::cerr << "ERROR: No device has been picked.. this is most likely a logical/code issue.. fix it" << std::endl;
        return;
    }

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (const QueueInfo& info : queueInfos) {
        // TODO: Should be done in initializer list.. but better for clarity now
        VkDeviceQueueCreateInfo ci;
        ci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        ci.pNext = nullptr;
        ci.flags = 0;
        ci.queueFamilyIndex = info.familyIndex;
        ci.queueCount = static_cast<uint32_t>(info.priorities.size());
        ci.pQueuePriorities = info.priorities.size() > 0 ? info.priorities.data() : nullptr;
        queueCreateInfos.push_back(ci);
    }

    // Logical device
    // TODO: Should be done in initializer list.. but better for clarity now
    VkDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.size() > 0 ? queueCreateInfos.data() : nullptr;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(desiredDeviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = desiredDeviceExtensions.size() > 0 ? &desiredDeviceExtensions[0] : nullptr;
    VkPhysicalDeviceFeatures desiredFeatures = GetPhysicalDeviceFeatures(selectedDevice); // TODO: This shouldn't be all.. should only be desired ones
    deviceCreateInfo.pEnabledFeatures = &desiredFeatures;
    // TODO: this approach could be used.. resets every member to 0 and sets the desired one to 1
    // if( !device_features.geometryShader ) { continue; } else { device_features = {}; device_features.geometryShader = VK_TRUE; }

    VkDevice logicalDevice = CreateLogicalDevice(selectedDevice, deviceCreateInfo);
    if (logicalDevice == VK_NULL_HANDLE) {
        std::cerr << "ERROR: Failed to create logical device" << std::endl;
        return;
    }
#else
    std::cerr << "You're trying to run VULKAN features, but haven't enabled it. Set DW_VULKAN_ENABLED to true in CMAKE to enable it." << std::endl;
#endif // DW_VULKAN_ENABLED
}

int main() {
    // Setup renderer
    // dw::InitData initData { dw::RendererType::RASTERIZER, dw::BackendType::VULKAN };
    // dw::PlatformData platformData = { nullptr };
    // auto engine = std::make_unique<dw::RenderEngine>(platformData, initData);

    testNonEngineIntergratedVulkan();

    std::cout << "\n\nPress enter to EXIT";
    std::cin.get();
    return 0;
}
