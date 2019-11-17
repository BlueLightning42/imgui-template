#include "pch.h"

#include "graphics.h"
#include "blu_config.h"

// https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Base_code


void Vulkan::setup_vulkan() {
	// in any setup step, if step fails it will std::exit() on a check_result
	create_instance();
	pick_physical_device();
	select_graphics_queue_family();
	create_logical_device();
	create_descriptor_pool();
}


// Private Methods
void Vulkan::create_instance() {
	// Get WSI extensions from SDL (we can add more if we like - we just can't remove these)
	unsigned extension_count;
	check_result(SDL_Vulkan_GetInstanceExtensions(this->_window, &extension_count, NULL),
		"Could not get the number of required instance extensions from SDL.");

	vec_cstr extensions(extension_count);
	check_result(SDL_Vulkan_GetInstanceExtensions(this->_window, &extension_count, extensions.data()),
		"Could not get the names of required instance extensions from SDL.");


	// Use validation layers if this is a debug build
	vec_cstr layers;
#if defined(_DEBUG)
	layers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif

	vk::ApplicationInfo appInfo = vk::ApplicationInfo()
		.setPApplicationName("Prison")
		.setApplicationVersion(VK_MAKE_VERSION(APP_MAJOR_VERSION, APP_MINOR_VERSION, APP_BLU_PATCH_VERSION))
		.setPEngineName("blu")
		.setEngineVersion(VK_MAKE_VERSION(ENG_MAJOR_VERSION, ENG_MINOR_VERSION, ENG_BLU_PATCH_VERSION))
		.setApiVersion(VK_API_VERSION_1_1)
		;

	vk::InstanceCreateInfo create_info = vk::InstanceCreateInfo()
		.setFlags(vk::InstanceCreateFlags())
		.setPApplicationInfo(&appInfo)
		.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()))
		.setPpEnabledExtensionNames(extensions.data())
		.setEnabledLayerCount(static_cast<uint32_t>(layers.size()))
		.setPpEnabledLayerNames(layers.data())
		;

	// Create the Vulkan instance.
	auto [result, instance] = vk::createInstance(create_info);
	check_result(result, "Error creating instance");
	this->_Instance = instance;
}

void Vulkan::create_surface() {
	// Create a Vulkan surface for rendering
	VkSurfaceKHR c_surface;

	check_result(SDL_Vulkan_CreateSurface(this->_window, c_Instance(), &c_surface),
		"Could not create a Vulkan surface.");

	vk::SurfaceKHR surface(c_surface);
	_surface = surface;

	// Check for WSI support
	auto result = _PhysicalDevice.getSurfaceSupportKHR(_QueueFamilyIndex, surface);
	check_result(result.result, "Could not initialize SDL. ERR: {}", fmt::make_format_args(SDL_GetError()));
	check_result((result.value == VK_TRUE), "Error no WSI support on physical device.");
}

//Thinking of swiching to a system where you can chose which device to use.
int Vulkan::rate_physical_device(const vk::PhysicalDevice &gpu) {
	if (!gpu.getFeatures().geometryShader) return -1;
	int score = 1;
	
	/*
	if (gpu.getProperties = vk::PhysicalDeviceProperties::)
	if (gpu.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}

	// Maximum possible size of textures affects graphics quality
	score += deviceProperties.limits.maxImageDimension2D;
	*/

	return score;
}

void Vulkan::pick_physical_device() {
	// select the physical GPU
	auto [result, gpus] = this->_Instance.enumeratePhysicalDevices();
	check_result(result, "Error Enumerating Physical Devices.");
	check_result((gpus.size() > 0), "No Physcial Devices Found!");
	// itterate over all gpu's returned and score them based on the rate physical device function.
	using gpu_pair = std::pair<int, vk::PhysicalDevice>;
	std::vector<gpu_pair> scored_physical_devices;
	for (const auto &gpu : gpus) {
		scored_physical_devices.push_back( {rate_physical_device(gpu), gpu} );
	}
	// pick the highest scored device
	auto res = std::max_element(scored_physical_devices.begin(), scored_physical_devices.end(),
		[ ](const gpu_pair& left, const gpu_pair& right) { 
				return left.first < right.first; 
		});
	check_result((res->first > 0), "No suitable Physical Devices Found!");
	//should find best gpu available based on capabilities.
	this->_PhysicalDevice = res->second;
}

uint32_t Vulkan::get_queue_index(vk::QueueFlagBits flag) {
	auto queues = this->_PhysicalDevice.getQueueFamilyProperties();

	for (uint32_t i = 0; i < queues.size(); ++i) {
		if (queues[i].queueFlags & flag) {
			return static_cast<uint32_t>(i);
		}
	}
	return (uint32_t)-1;
}

void Vulkan::select_graphics_queue_family() {
	this->_QueueFamilyIndex = get_queue_index(vk::QueueFlagBits::eGraphics);
	check_result((this->_QueueFamilyIndex != (uint32_t)-1), "Graphics Queue Family Error: Index is bad");
}


void Vulkan::create_logical_device() {
	int device_extension_count = 1;
	const char* device_extensions[ ] = {"VK_KHR_swapchain"};
	const float queue_priority[ ] = {1.0f};	


	vk::DeviceQueueCreateInfo queue_info[1] = {vk::DeviceQueueCreateInfo()
		.setQueueCount(1)
		.setPQueuePriorities(queue_priority)
		.setQueueFamilyIndex(this->_QueueFamilyIndex)}
	;

	vk::DeviceCreateInfo create_info = vk::DeviceCreateInfo()
		.setQueueCreateInfoCount(1)
		.setPQueueCreateInfos(queue_info)
		.setEnabledExtensionCount(device_extension_count)
		.setPpEnabledExtensionNames(device_extensions)
		;

	auto [result, device] = this->_PhysicalDevice.createDevice(create_info);
	check_result(result, "Problem with creating device");
	this->_Device = device;
	this->_Queue = device.getQueue(this->_QueueFamilyIndex, 0);
}

void Vulkan::create_descriptor_pool() {
	using DescriptorPoolS = vk::DescriptorPoolSize;
	using SizeType = vk::DescriptorType;
	std::vector<vk::DescriptorPoolSize> descriptorPoolSizes = {
		DescriptorPoolS(SizeType::eSampler,					1000),
		DescriptorPoolS(SizeType::eCombinedImageSampler,	1000),
		DescriptorPoolS(SizeType::eSampledImage,			1000),
		DescriptorPoolS(SizeType::eStorageBuffer,			1000),
		DescriptorPoolS(SizeType::eStorageImage,			1000),
		DescriptorPoolS(SizeType::eUniformTexelBuffer,		1000),
		DescriptorPoolS(SizeType::eStorageTexelBuffer,		1000),
		DescriptorPoolS(SizeType::eUniformBuffer,			1000),
		DescriptorPoolS(SizeType::eStorageBufferDynamic,	1000),
		DescriptorPoolS(SizeType::eUniformBufferDynamic,	1000),
		DescriptorPoolS(SizeType::eInputAttachment,			1000)
	};

	vk::DescriptorPoolCreateInfo pool_info = vk::DescriptorPoolCreateInfo()
		.setFlags(vk::DescriptorPoolCreateFlags::Flags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT))
		.setPoolSizeCount(descriptorPoolSizes.size())
		.setPPoolSizes(descriptorPoolSizes.data())
		.setMaxSets(1000 * descriptorPoolSizes.size())
		;

	auto [result, descriptorPool] = _Device.createDescriptorPool(pool_info, _Allocator);
	check_result(result, "Error Creating Descriptor Pool");
	this->_DescriptorPool = descriptorPool;
}
