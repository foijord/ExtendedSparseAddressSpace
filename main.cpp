#include <vulkan/vulkan.h>

#include <cmath>
#include <vector>
#include <chrono>
#include <iostream>

class Timer {
public:
	Timer(std::string message) :
		message(std::move(message)),
		start_time(std::chrono::steady_clock::now())
	{}

	~Timer()
	{
		auto seconds = std::chrono::duration_cast<std::chrono::duration<double>>(
			std::chrono::steady_clock::now() - this->start_time).count();

		std::cout << message << " " << seconds << " seconds" << std::endl;
	}

	std::string message;
	std::chrono::time_point<std::chrono::steady_clock> start_time;
};

int main()
{
    VkApplicationInfo applicationInfo{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = nullptr,
		.pApplicationName = "Extended Sparse Address Space Test",
		.applicationVersion = 1,
		.pEngineName = "Extended Sparse Address Space",
		.engineVersion = 1,
		.apiVersion = VK_API_VERSION_1_0,
	};

	std::vector<const char*> instance_layers{};
	std::vector<const char*> instance_extensions{
		VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
	};

	VkInstanceCreateInfo instanceCreateInfo{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pApplicationInfo = &applicationInfo,
		.enabledLayerCount = static_cast<uint32_t>(instance_layers.size()),
		.ppEnabledLayerNames = instance_layers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(instance_extensions.size()),
		.ppEnabledExtensionNames = instance_extensions.data()
	};

	VkInstance instance;
	if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
		std::cerr << "vkCreateInstance failed!" << std::endl;
		return EXIT_FAILURE;
	}

	uint32_t physicalDeviceCount;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

	auto physicalDevice = physicalDevices.front();

	VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV physicalDeviceExtendedAddressSpaceFeatures{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_SPARSE_ADDRESS_SPACE_FEATURES_NV,
	};

	VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
		.pNext = &physicalDeviceExtendedAddressSpaceFeatures,
	};

	vkGetPhysicalDeviceFeatures2(physicalDevice, &physicalDeviceFeatures2);

	if (physicalDeviceFeatures2.features.sparseBinding != VK_TRUE) {
		std::cerr << "sparseBinding not supported!" << std::endl;
		return EXIT_FAILURE;
	}

	if (physicalDeviceFeatures2.features.sparseResidencyImage3D != VK_TRUE) {
		std::cerr << "sparseResidencyImage3D not supported!" << std::endl;
		return EXIT_FAILURE;
	}

	if (physicalDeviceExtendedAddressSpaceFeatures.extendedSparseAddressSpace != VK_TRUE) {
		std::cerr << "physicalDeviceExtendedAddressSpaceFeatures.extendedSparseAddressSpace != VK_TRUE" << std::endl;
		return EXIT_FAILURE;
	}

	VkPhysicalDeviceExtendedSparseAddressSpacePropertiesNV physicalDeviceExtendedSparseAddressSpaceProperties{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_SPARSE_ADDRESS_SPACE_PROPERTIES_NV,
		.pNext = nullptr,
	};

	VkPhysicalDeviceProperties2 physicalDeviceProperties2{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR,
		.pNext = &physicalDeviceExtendedSparseAddressSpaceProperties,
	};

	vkGetPhysicalDeviceProperties2(physicalDevice, &physicalDeviceProperties2);

	std::cout << "Found device: " << physicalDeviceProperties2.properties.deviceName << std::endl;

	std::cout << "NVIDIA Driver version: " <<
		((physicalDeviceProperties2.properties.driverVersion >> 22) & 0x3ff) << "." <<
		((physicalDeviceProperties2.properties.driverVersion >> 14) & 0x0ff) << "." <<
		((physicalDeviceProperties2.properties.driverVersion >> 6) & 0x0ff) << "." <<
		((physicalDeviceProperties2.properties.driverVersion) & 0x003f) << std::endl;

	VkFormat imageformat = VK_FORMAT_R8_UNORM;
	VkImageType imageType = VK_IMAGE_TYPE_3D;
	VkImageTiling imagetiling = VK_IMAGE_TILING_OPTIMAL;
	VkImageUsageFlags imageusageflags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	VkImageCreateFlags imagecreateflags = VK_IMAGE_CREATE_SPARSE_BINDING_BIT | VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT;

	VkImageFormatProperties imageFormatProperties;
	if (vkGetPhysicalDeviceImageFormatProperties(physicalDevice, imageformat, imageType, imagetiling, imageusageflags, imagecreateflags, &imageFormatProperties) != VK_SUCCESS) {
		std::cerr << "vkGetPhysicalDeviceImageFormatProperties failed!";
		return EXIT_FAILURE;
	}

	if ((imageusageflags & physicalDeviceExtendedSparseAddressSpaceProperties.extendedSparseImageUsageFlags) != imageusageflags) {
		std::cerr << "unsupported image usage flags.";
		return EXIT_FAILURE;
	}

	std::cout << "VkImageFormatProperties.maxExtent = { " <<
		imageFormatProperties.maxExtent.width << ", " <<
		imageFormatProperties.maxExtent.height << ", " <<
		imageFormatProperties.maxExtent.depth << " }" << std::endl;

	std::cout << "VkPhysicalDeviceProperties2.properties.limits.maxImageDimension3D = " <<
		physicalDeviceProperties2.properties.limits.maxImageDimension3D << std::endl;

	std::cout << "VkImageFormatProperties.maxResourceSize = " <<
		imageFormatProperties.maxResourceSize << std::endl;

	std::cout << "VkPhysicalDeviceProperties2.properties.limits.sparseAddressSpaceSize = " << 
		physicalDeviceProperties2.properties.limits.sparseAddressSpaceSize << std::endl;

	std::cout << "VkPhysicalDeviceExtendedSparseAddressSpacePropertiesNV.extendedSparseAddressSpaceSize = " <<
		physicalDeviceExtendedSparseAddressSpaceProperties.extendedSparseAddressSpaceSize << std::endl;

	std::vector<const char*> device_layers{};
	std::vector<const char*> device_extensions{
		VK_NV_EXTENDED_SPARSE_ADDRESS_SPACE_EXTENSION_NAME,
	};

	std::vector<float> queuePriorities = { 1.0f };

	std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos{ {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.queueFamilyIndex = 0,
		.queueCount = static_cast<uint32_t>(queuePriorities.size()),
		.pQueuePriorities = queuePriorities.data()
	} };

	VkDeviceCreateInfo deviceCreateInfo{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = &physicalDeviceFeatures2,
		.flags = 0,
		.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size()),
		.pQueueCreateInfos = deviceQueueCreateInfos.data(),
		.enabledLayerCount = static_cast<uint32_t>(device_layers.size()),
		.ppEnabledLayerNames = device_layers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size()),
		.ppEnabledExtensionNames = device_extensions.data(),
		.pEnabledFeatures = nullptr,
	};

	VkDevice device;
	if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
		std::cerr << "vkCreateDevice failed: " << std::endl;
		return EXIT_FAILURE;
	}

	VkImageCreateInfo imageCreateInfo{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = nullptr,
		.flags = imagecreateflags,
		.imageType = imageType,
		.format = imageformat,
		.extent = imageFormatProperties.maxExtent,
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = imagetiling,
		.usage = imageusageflags,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};

	auto imagesize = 
		VkDeviceSize(imageFormatProperties.maxExtent.width) *
		VkDeviceSize(imageFormatProperties.maxExtent.height) *
		VkDeviceSize(imageFormatProperties.maxExtent.depth);

	auto numimages = physicalDeviceExtendedSparseAddressSpaceProperties.extendedSparseAddressSpaceSize / imagesize;

	std::cout << "Creating " << numimages << " images with extent = { " <<
		imageCreateInfo.extent.width << ", " <<
		imageCreateInfo.extent.height << ", " <<
		imageCreateInfo.extent.depth << " }: " << std::endl;


	std::vector<VkImage> images(numimages, nullptr);
	for (auto& image : images) {
		Timer timer("image created in ");
		if (vkCreateImage(device, &imageCreateInfo, nullptr, &image) != VK_SUCCESS) {
			std::cerr << "image creation failed." << std::endl;
			break;
		}
	}
	for (auto& image : images) {
		if (image) {
			vkDestroyImage(device, image, nullptr);
		}
	}
	return EXIT_SUCCESS;
}
