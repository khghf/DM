#include<Core/RHI/Backend/Vulkan/VulkanDevice.h>
#include<Core/RHI/Backend/Vulkan/VulkanRenderPass.h>
#include<Core/RHI/Backend/Vulkan/VulkanSwapchain.h>
#include<Core/RHI/Backend/Vulkan/Buffer/VulkanBuffer.h>
#include<Core/RHI/Backend/Vulkan/VulkanShader.h>
#include<Core/RHI/Backend/Vulkan/VulkanShaderProgram.h>
#include<Core/RHI/Backend/Vulkan/VulkanPipeline.h>
#include<Core/RHI/Backend/Vulkan/VulkanCommandList.h>
#include<Core/RHI/Backend/Vulkan/Buffer/VulkanVertexBuffer.h>
#include<Core/RHI/Backend/Vulkan/Buffer/VulkanIndexBuffer.h>
#include<Core/RHI/Backend/Vulkan/Buffer/VulkanUniformBuffer.h>
#include<Core/RHI/Backend/Vulkan/VulkanTexture.h>
#include<GLFW/glfw3.h>
#include<vector>
#include<set>
#include<iostream>



namespace DM::RHI
{

	const std::vector<const char*> g_ValidationLayers = 
	{ 
		"VK_LAYER_KHRONOS_validation" 
	};
	const std::vector<const char*> DeviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	VulkanDevice::VulkanDevice(const RHIDeviceDesc& desc)
	{
		m_WindowHandle = desc.WindowHandle;
		CreatevkInstance(desc.bEnableValidation); 
		SetupDebugMessenger();                  
		CreatevkSurface();                        
		PickPhysicalDevice();                   
		CreatevkDevice();       
		CreateSingleTimevkCommandPool();
		CreateSyncObjects();
		CreatevkDescriptorPool();
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDestroyDescriptorPool(m_vkDevice, m_vkDescriptorPool, nullptr);
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			vkDestroySemaphore(m_vkDevice, m_RenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(m_vkDevice, m_ImageAvailableSemaphores[i], nullptr);
			vkDestroyFence(m_vkDevice, m_InFlightFences[i], nullptr);
		}

		vkDestroyCommandPool(m_vkDevice, m_SingleTimeCommandPool, nullptr);
		vkDestroyDevice(m_vkDevice, nullptr);
		if (m_vkDebugMessenger)
		{
			// 调试回调的销毁函数不是静态链接的，要通过实例地址查出来再调(Vulkan 扩展函数都这样取)。
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vkInstance, "vkDestroyDebugUtilsMessengerEXT");
			if (func) func(m_vkInstance, m_vkDebugMessenger, nullptr);
		}
		vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, nullptr);
		vkDestroyInstance(m_vkInstance, nullptr);

	}

	void VulkanDevice::BeginFrame()
	{
		vkWaitForFences(m_vkDevice, 1, GetInFlightFence(), VK_TRUE, UINT64_MAX);
		vkResetFences(m_vkDevice, 1, GetInFlightFence());
	}

	void VulkanDevice::EndFrame()
	{
		CURRENT_CPU_PROCESSES_FRAME_INDEX = (CURRENT_CPU_PROCESSES_FRAME_INDEX + 1) % MAX_FRAMES_IN_FLIGHT;
		m_bIsFirstRenderFrame = false;
	}

	void VulkanDevice::WaitGPUIdle()
	{
		vkDeviceWaitIdle(m_vkDevice); //先等GPU把活全干完，防止引用已销毁资源
	}

	RHIRenderPass* VulkanDevice::CreateRenderPass(const RHIRenderPassDesc& desc)
	{
		return new VulkanRenderPass(this, desc);
	}

	RHISwapchain* VulkanDevice::CreateSwapchain(const RHISwapchainDesc& desc)
	{
		return new VulkanSwapchain(this, desc);
	}
	

	RHIShader* VulkanDevice::CreateShader(const RHIShaderDesc& desc)
	{
		return new VulkanShader(this, desc);
	}

	RHIShaderProgram* VulkanDevice::CreateShaderProgram(const RHIShaderProgramDesc& desc)
	{
		return new VulkanShaderProgram(this,desc);
	}

	RHIPipeline* VulkanDevice::CreatePipeline(const RHIPipelineDesc& desc)
	{
		return new VulkanPipeline(this, desc);
	}


	RHIBuffer* VulkanDevice::CreateVertexBuffer(const RHIVertexBufferDesc& desc)
	{
		return new VulkanVertexBuffer(this, desc);
	}

	RHIBuffer* VulkanDevice::CreateIndexBuffer(const RHIIndexBufferDesc& desc)
	{
		return new VulkanIndexBuffer(this, desc);
	}

	RHIBuffer* VulkanDevice::CreateUniformBuffer(const RHIUniformBufferDesc& desc)
	{
		return new VulkanUniformBuffer(this, desc);
	}

	RHITexture* VulkanDevice::CreateTexture(const RHITextureDesc& desc)
	{
		return new VulkanTexture(this,desc,nullptr);
	}

	RHITexture* VulkanDevice::CreateTexture(const RHITextureDesc& desc, const void* data)
	{
		return new VulkanTexture(this, desc, data);
	}





	RHICommandList* VulkanDevice::CreateCommandList()
	{
		return new VulkanCommandList(this);
	}



	void VulkanDevice::CreatevkInstance(bool enableValidation)
	{
		VkApplicationInfo appInfo{}; // 应用信息(名字/版本)，对功能没影响，便于驱动统计。
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "DM Engine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "DM-RHI";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_2; 

		auto extensions = GetRequiredExtensions();
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		if (enableValidation && CheckValidationLayerSupport(g_ValidationLayers))
		{
			// 开启验证层：开发期能抓出几乎所有误用。
			createInfo.enabledLayerCount = static_cast<uint32_t>(g_ValidationLayers.size());
			createInfo.ppEnabledLayerNames = g_ValidationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0; // 没装 SDK 或不想开，就关掉，避免创建失败
		}

		VK_CHECK(vkCreateInstance(&createInfo, nullptr, &m_vkInstance));
	}

	

	void VulkanDevice::SetupDebugMessenger()
	{
		VkDebugUtilsMessengerCreateInfoEXT ci{};
		ci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		// 只对 WARNING 和 ERROR 级别感兴趣(防止被一堆 INFO 刷屏)。
		ci.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		// 三类消息：通用 / 校验/ 性能。
		ci.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		ci.pfnUserCallback = DebugCallback;

		// 扩展函数，需要先查地址再调用。
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vkInstance, "vkCreateDebugUtilsMessengerEXT");
		if (func) func(m_vkInstance, &ci, nullptr, &m_vkDebugMessenger);
	}

	// ------------------------------------------------------------
	// 窗口系统：用 GLFW(glfw会帮我们处理好各种平台差异)把窗口句柄连成 VkSurfaceKHR(Vulkan 与窗口之间的桥)
	//  "能不能把这个窗口用来呈现"必须由 Surface 来确认，所以 Surface 必须先于选 GPU 建好。
	// ------------------------------------------------------------
	void VulkanDevice::CreatevkSurface()
	{
		VK_CHECK(glfwCreateWindowSurface(m_vkInstance, static_cast<GLFWwindow*>(m_WindowHandle), nullptr, &m_vkSurface));
	}

	void VulkanDevice::PickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, nullptr);
		if (deviceCount == 0) throw std::runtime_error("找不到任何 Vulkan 物理设备(GPU)");
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, devices.data());

		// 取第一张"够用"的 GPU。
		for (const auto& d : devices)
		{
			if (IsDeviceSuitable(d, m_vkSurface)) { m_vkPhysicalDevice = d; break; }
		}
		if (m_vkPhysicalDevice == VK_NULL_HANDLE)throw std::runtime_error("没有满足条件的 GPU(需支持图形+呈现+交换链)");
	}

	void VulkanDevice::CreatevkDevice()
	{
		QueueFamilyIndices indices = FindQueueFamilies(m_vkPhysicalDevice, m_vkSurface);
		m_vkGraphicsFamily = indices.graphics.value();
		m_vkPresentFamily = indices.present.value();

		// 图形族与呈现族可能相同，用 set 去重，只对"真正需要的族"各获取一个队列。
		std::set<uint32_t> uniqueFamilies = { indices.graphics.value(), indices.present.value()};
		std::vector<VkDeviceQueueCreateInfo> queueCIs;
		float queuePriority = 1.0f; // 队列优先级，单个队列随便给 1.0
		for (uint32_t family : uniqueFamilies)
		{
			VkDeviceQueueCreateInfo qci{};
			qci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			qci.queueFamilyIndex = family;
			qci.queueCount = 1;
			qci.pQueuePriorities = &queuePriority;
			queueCIs.push_back(qci);
		}

		VkPhysicalDeviceFeatures deviceFeatures{}; 
		deviceFeatures.samplerAnisotropy = VK_TRUE;//启用各项异性
		deviceFeatures.sampleRateShading = VK_TRUE;

		VkDeviceCreateInfo devCI{};
		devCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		devCI.queueCreateInfoCount = static_cast<uint32_t>(queueCIs.size());
		devCI.pQueueCreateInfos = queueCIs.data();
		devCI.pEnabledFeatures = &deviceFeatures;

		// 逻辑设备必须启用交换链扩展(呈现要用)。
		devCI.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());
		devCI.ppEnabledExtensionNames = DeviceExtensions.data();
		devCI.enabledLayerCount = 0; // 现代 Vulkan 中设备层已废弃，验证层只在实例层启用

		VK_CHECK(vkCreateDevice(m_vkPhysicalDevice, &devCI, nullptr, &m_vkDevice));

		// 从逻辑设备里把"图形队列"和"呈现队列"这两个句柄取出来(提交/呈现时用)。
		vkGetDeviceQueue(m_vkDevice, indices.graphics.value(), 0, &m_vkGraphicsQueue);
		vkGetDeviceQueue(m_vkDevice, indices.present.value(), 0, &m_vkPresentQueue);
	}

	void VulkanDevice::CreateSingleTimevkCommandPool()
	{
		// 创建一个专用于一次性命令的池
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = m_vkGraphicsFamily;
		VK_CHECK(vkCreateCommandPool(m_vkDevice, &poolInfo, nullptr, &m_SingleTimeCommandPool));

		// 预先分配一个命令缓冲区
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_SingleTimeCommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;
		VK_CHECK(vkAllocateCommandBuffers(m_vkDevice, &allocInfo, &m_SingleTimeCommandBuffer));
	}

	

	void VulkanDevice::CreateSyncObjects()
	{
		VkSemaphoreCreateInfo sci;
		sci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		sci.pNext = nullptr;
		sci.flags = 0;
		m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

		VkFenceCreateInfo fci;
		fci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fci.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		fci.pNext = nullptr;

		m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			VK_CHECK(vkCreateSemaphore(m_vkDevice, &sci, nullptr, &m_RenderFinishedSemaphores[i]));
			VK_CHECK(vkCreateSemaphore(m_vkDevice, &sci, nullptr, &m_ImageAvailableSemaphores[i]));
			VK_CHECK(vkCreateFence(m_vkDevice, &fci, nullptr, &m_InFlightFences[i]));
		}
	}

	void VulkanDevice::CreatevkDescriptorPool()
	{
		// ============================================================
		// 描述符池库存配置
		// 每个 VkDescriptorPoolSize 声明一种描述符类型的总数量
		// 所有从该池分配的描述符集共享这些配额
		// ============================================================
		std::vector<VkDescriptorPoolSize> poolSizes =
		{
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 200 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100 },
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 100 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 50 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 50 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 200 },
		};

		VkDescriptorPoolCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

		info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

		// 该池最多能分配的描述符集(VkDescriptorSet)数量
		info.maxSets = 1000;
		info.poolSizeCount = poolSizes.size();

		info.pPoolSizes = poolSizes.data();

		VK_CHECK(vkCreateDescriptorPool(m_vkDevice, &info, nullptr, &m_vkDescriptorPool));
	}
	
	uint32_t VulkanDevice::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
	{
		VkPhysicalDeviceMemoryProperties memProps;
		vkGetPhysicalDeviceMemoryProperties(m_vkPhysicalDevice, &memProps);
		for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i)
		{
			// 第 i 位在 typeFilter 中置位(GPU 允许用于该资源)，
			// 且该内存类型具备我们想要的属性(按位全包含)。
			if ((typeFilter & (1u << i)) &&(memProps.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		}
		throw std::runtime_error("failed to find suitable memory type!");
	}

	SwapChainSupportDetails VulkanDevice::QueryvkSwapChainSupport(VkPhysicalDevice device)const
	{
		// 能力(capabilities)、格式(formats)、呈现模式(presentModes)。
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, GetvkSurface(), &details.capabilities);

		uint32_t fmtCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, GetvkSurface(), &fmtCount, nullptr);
		if (fmtCount)
		{
			details.formats.resize(fmtCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, GetvkSurface(), &fmtCount, details.formats.data());
		}

		uint32_t modeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, GetvkSurface(), &modeCount, nullptr);
		if (modeCount)
		{
			details.presentModes.resize(modeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, GetvkSurface(), &modeCount, details.presentModes.data());
		}

		return details;
	}

	
	VkSurfaceFormatKHR VulkanDevice::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)const
	{
		// 优先选 B8G8R8A8 + sRGB
		for (const auto& f : formats)
		{
			if (f.format == VK_FORMAT_B8G8R8A8_UNORM && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)return f;
		}

		return formats.empty() ? VkSurfaceFormatKHR{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR } : formats[0];
	}

	VkSampleCountFlagBits VulkanDevice::GetMaxUsableSampleCount()const
	{
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(GetvkPhysicalDevice(), &physicalDeviceProperties);

		VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
		if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
		if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
		if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
		if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

		return VK_SAMPLE_COUNT_1_BIT;
	}

	uint32_t VulkanDevice::GetMaxPushConstantsSize() const
	{
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(m_vkPhysicalDevice, &properties);
		return properties.limits.maxPushConstantsSize;
	}

	VkDescriptorSet VulkanDevice::AllocateDescriptorSet(const VkDescriptorSetLayout* setLayout) const
	{
		return AllocateDescriptorSets(setLayout,1)[0];
	}

	std::vector<VkDescriptorSet> VulkanDevice::AllocateDescriptorSets(const VkDescriptorSetLayout* setLayout, uint32_t count) const
	{
		std::vector<VkDescriptorSet>sets;
		sets.resize(count);

		VkDescriptorSetAllocateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		info.descriptorPool = m_vkDescriptorPool;
		info.descriptorSetCount = count;
		info.pSetLayouts = setLayout;

		vkAllocateDescriptorSets(m_vkDevice, &info, sets.data());

		return sets;
	}

	void VulkanDevice::FreeDescriptorSet(const VkDescriptorSet* set) const
	{
		FreeDescriptorSets(set, 1);
	}

	void VulkanDevice::FreeDescriptorSets(const VkDescriptorSet* sets, uint32_t count) const
	{
		vkFreeDescriptorSets(m_vkDevice, m_vkDescriptorPool, count, sets);

	}


	VkCommandBuffer VulkanDevice::BeginSingleTimeCommands()
	{
		VK_CHECK(vkResetCommandBuffer(m_SingleTimeCommandBuffer, 0));
		VkCommandBufferBeginInfo info{};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VK_CHECK(vkBeginCommandBuffer(m_SingleTimeCommandBuffer, &info));
		return m_SingleTimeCommandBuffer;
	}

	void VulkanDevice::EndSingleTimeCommands()
	{
		VK_CHECK(vkEndCommandBuffer(m_SingleTimeCommandBuffer));
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_SingleTimeCommandBuffer;

		vkQueueSubmit(m_vkGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_vkGraphicsQueue);
	}
	
	void VulkanDevice::CreatevkBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)const
	{

		//创建缓冲
		{
			VkBufferCreateInfo ci{};
			ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			ci.usage = usage;
			ci.size = size;
			ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			(vkCreateBuffer(GetvkDevice(), &ci, nullptr, &buffer));
		}
		{
			// 询问这块缓冲需要多少显存、能用哪些内存类型。
			VkMemoryRequirements req{};
			vkGetBufferMemoryRequirements(GetvkDevice(), buffer, &req);

			VkMemoryAllocateInfo ci{};
			ci.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			ci.allocationSize = req.size;
			ci.memoryTypeIndex = FindMemoryType(req.memoryTypeBits, properties);
			(vkAllocateMemory(GetvkDevice(), &ci, nullptr, &bufferMemory));
		}
		(vkBindBufferMemory(GetvkDevice(), buffer, bufferMemory, 0)); // 把显存绑到缓冲
	}

	void VulkanDevice::CreatevkImage(VkImageType imageType, uint32_t width, uint32_t height, VkFormat format, VkSampleCountFlagBits sampleCount, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, uint32_t depth, uint32_t mipLevels, uint32_t arrayLayers) const
	{
		//创建图像
		VkDeviceMemory textureImageMemory;

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = imageType;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = depth;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = arrayLayers;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = sampleCount;
		imageInfo.flags = 0; // Optional
		if (vkCreateImage(GetvkDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}
		//申请并绑定设备内存
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(GetvkDevice(), image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(GetvkDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(GetvkDevice(), image, imageMemory, 0);
	}
	void VulkanDevice::CreatevkImageView(VkImage image, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspectMask, VkImageView&imageView,uint32_t baseMipLevel,uint32_t levelCount,uint32_t baseArrayLayer,uint32_t layerCount)const
	{
		//创建图像视图
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = viewType;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectMask;
		viewInfo.subresourceRange.baseMipLevel = baseMipLevel;
		viewInfo.subresourceRange.levelCount = levelCount;
		viewInfo.subresourceRange.baseArrayLayer = baseArrayLayer;
		viewInfo.subresourceRange.layerCount = layerCount;
		if (vkCreateImageView(GetvkDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}
	}

	void VulkanDevice::TransitionImageLayout(VkCommandBuffer cmdBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask, uint32_t baseMipLevel, uint32_t levelCount, uint32_t baseArrayLayer, uint32_t layerCount)
	{
		VkCommandBuffer commandBuffer = cmdBuffer;
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;      // 从第几级 Mipmap 开始
		barrier.subresourceRange.levelCount = 1;        // 影响多少级 Mipmap
		barrier.subresourceRange.baseArrayLayer = 0;    // 从第几层纹理数组开始
		barrier.subresourceRange.layerCount = 1;        // 影响多少层

		VkPipelineStageFlags sourceStage;        // 屏障前一个操作发生的管线阶段
		VkPipelineStageFlags destinationStage;   // 屏障后一个操作发生的管线阶段

		// UNDEFINED -> TRANSFER_DST(准备拷贝进图像)
		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			barrier.srcAccessMask = 0;  // 之前没有任何访问

			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;  // 之后要写入
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			barrier.srcAccessMask = 0;  // 之前没有任何访问

			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;  // 之后着色器要读取
		}
		// TRANSFER_DST -> SHADER_READ_ONLY(拷贝完准备采样)
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;  // 等写入完成

			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;     // 之后要读取
		}

		// PRESENT_SRC -> TRANSFER_DST(呈现完准备拷贝进)
		else if (oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			barrier.srcAccessMask = 0;  // 呈现完成，无后续访问

			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;  // 之后要写入
		}

		// TRANSFER_DST -> PRESENT_SRC(拷贝完准备呈现)
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		{
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;  // 等写入完成

			destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			barrier.dstAccessMask = 0;  // 呈现阶段不需要特定访问掩码
		}

		// COLOR_ATTACHMENT -> TRANSFER_SRC(渲染完准备拷贝出)
		else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
		{
			sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;  // 等颜色写入完成

			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;  // 之后要读取
		}

		//TRANSFER_SRC -> SHADER_READ_ONLY(拷贝出后准备采样)
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;  // 等读取完成

			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;    // 之后要读取
		}

		// TRANSFER_SRC -> PRESENT_SRC(拷贝出后准备呈现)
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		{
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;  // 等读取完成

			destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			barrier.dstAccessMask = 0;
		}
		else
		{
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

	}

	void VulkanDevice::TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask, uint32_t baseMipLevel,uint32_t levelCount,uint32_t baseArrayLayer,uint32_t layerCount)
	{
		VkCommandBuffer cmd = BeginSingleTimeCommands();
		TransitionImageLayout(cmd, image, oldLayout, newLayout, aspectMask, baseMipLevel, levelCount, baseArrayLayer, layerCount);
		EndSingleTimeCommands();
	}

	void VulkanDevice::CopyvkBufferTovkImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
	{
		VkCommandBuffer commandBuffer =BeginSingleTimeCommands();
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width,height,1 };
		vkCmdCopyBufferToImage(
			commandBuffer,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);
		EndSingleTimeCommands();
	}

	

	VkFormat VulkanDevice::FindDepthFormat() const
	{
		return FindSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	VkFormat VulkanDevice::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)const
	{
		for (VkFormat format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(GetvkPhysicalDevice(), format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) return format;
				
			if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)return format; 
		}
		throw std::runtime_error("failed to find supported format!");
		return {};
	}

	



	bool VulkanDevice::CheckValidationLayerSupport(const std::vector<const char*>& layers)
	{
		// 枚举本机装了哪些验证层，确认我们要的那个存在(Vulkan SDK 装上就有)。
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> available(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, available.data());
		for (const char* need : layers)
		{
			bool found = false;
			for (const auto& prop : available)
				if (strcmp(prop.layerName, need) == 0) { found = true; break; }
			if (!found) return false;
		}
		return true;
	}


	std::vector<const char*> VulkanDevice::GetRequiredExtensions()
	{
		// GLFW 告诉我们：要让 Vulkan 连上这个窗口系统，需要哪些"实例扩展"。
		uint32_t glfwExtCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtCount);
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // 调试回调要用
		return extensions;
	}

	QueueFamilyIndices VulkanDevice::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		//  遍历 GPU 的"队列族"，记下两个关键索引：
		//   支持 VK_QUEUE_GRAPHICS_BIT 的 = 能画图；
		//   能支持把图呈现到 surface 的 = 能上屏。
		QueueFamilyIndices indices;
		uint32_t familyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
		std::vector<VkQueueFamilyProperties> families(familyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, families.data());

		for (uint32_t i = 0; i < familyCount; ++i)
		{
			if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)indices.graphics = i;
			// 必须有 Surface 才能判断"该族能否呈现"(这就是为什么 Surface 必须先建)。
			VkBool32 presentSupport = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			if (presentSupport) indices.present = i;
			if (indices.IsComplete()) break;
		}
		return indices;
	}

	bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		QueueFamilyIndices indices = FindQueueFamilies(device, surface);
		if (!indices.IsComplete()) return false;

		// 必须支持交换链扩展，且至少有可用格式/呈现模式，否则无法呈现。
		uint32_t extCount = 0;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);
		std::vector<VkExtensionProperties> exts(extCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, exts.data());

		bool swapchainOK = false;
		for (const auto& e : exts)if (strcmp(e.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) swapchainOK = true;
		if (!swapchainOK) return false;
		//查询交换链格式支持
		SwapChainSupportDetails support = QueryvkSwapChainSupport(device);
		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		//满足格式要求、呈现模式要求、支持各向异性
		return !support.formats.empty() && !support.presentModes.empty()&& supportedFeatures.samplerAnisotropy;
	}
	
	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDevice::DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT type,
		const VkDebugUtilsMessengerCallbackDataEXT* data,
		void* /*userData*/)
	{
		// 着色
		const char* color = "";
		const char* label = "";

		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			color = "\033[31m"; label = "ERROR";
		}
		else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			color = "\033[33m"; label = "WARNING";
		}
		else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
			color = "\033[36m"; label = "INFO";
		}

		std::cerr << color << "[" << label << "]\033[0m " << data->pMessage << "\n";

		if (data->pMessageIdName) {
			std::cerr << "  VUID: " << data->pMessageIdName << "\n";
		}

		for (uint32_t i = 0; i < data->objectCount; i++) {
			auto& obj = data->pObjects[i];
			std::cerr << "  [" << i << "] Type=" << obj.objectType
				<< " Handle=0x" << std::hex << obj.objectHandle;
			if (obj.pObjectName) std::cerr << " Name=\"" << obj.pObjectName << "\"";
			std::cerr << std::dec << "\n";
		}

#ifdef _DEBUG
		if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			//__debugbreak();
		}
#endif
		return VK_FALSE;
	}

} // namespace DM::RHI
