#pragma once
#include"Core/RHI/RHIDevice.h"
#include<vulkan/vulkan.h>
#include<stdexcept>
#include<string>
#include<vector>
#include<cstdint>

#ifndef VK_CHECK
#define VK_CHECK(x)                                                                   \
	{                                                                               \
		VkResult _vkResult = (x);                                                      \
		if (_vkResult != VK_SUCCESS)                                                   \
		{                                                                              \
			throw std::runtime_error(std::string("[Vulkan] call failed code=") +          \
				std::to_string(_vkResult) + " @ " + std::string(__FILE__) + ":" +      \
				std::to_string(__LINE__));                                             \
		}                                                                              \
	} 
#endif

namespace DM::RHI
{
	/// <summary>
	/// 描述交换链支持的详细信息。
	/// </summary>
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR        capabilities = {};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR>   presentModes;
	};

	/// <summary>
	/// 表示最大并行帧数。
	/// </summary>
	constexpr inline uint8_t MAX_FRAMES_IN_FLIGHT = 2;
	/// <summary>
	/// 用于表示当前 CPU正在处理的帧的索引。
	/// </summary>
	inline uint8_t CURRENT_CPU_PROCESSES_FRAME_INDEX=0;

	/// <summary>
	/// 队列族(Queue Family)
	/// GPU 的队列按"能力"分成不同的"族"。一张显卡可能：有的族能画图(graphics)，有的族能把图呈现到屏幕(present)，有的只擅长搬数据(transfer)。
	/// </summary>
	struct QueueFamilyIndices
	{
		std::optional<uint32_t>graphics;
		std::optional<uint32_t>present;
		bool IsComplete() const { return graphics.has_value() && present.has_value(); }
	};
	class VulkanSwapchain;
	/// <summary>
	/// VulkanDevice 是一个继承自 RHIDevice 的类，用于封装 Vulkan API 的设备管理和操作。
	/// 提供一些必须依赖vulkan核心成员才能实现的接口、以及各种资源的创建。
	/// </summary>
	class DM_API VulkanDevice : public RHIDevice
	{
	public:
		explicit VulkanDevice(const RHIDeviceDesc& desc);

		 ~VulkanDevice() ;
		// ---- RHIDevice接口----
		virtual void BeginFrame() override;
		virtual void EndFrame()override;
		virtual void WaitGPUIdle() override;
		virtual RHIRenderPass*		CreateRenderPass(const RHIRenderPassDesc& desc)override;
		virtual RHISwapchain*		CreateSwapchain(const RHISwapchainDesc& desc) override;
		virtual RHIShader*			CreateShader(const RHIShaderDesc& desc) override;
		virtual RHIShaderProgram*	CreateShaderProgram(const RHIShaderProgramDesc& desc) override;
		virtual RHIPipeline*		CreatePipeline(const RHIPipelineDesc& desc) override;

		virtual RHIBuffer*			CreateBuffer(const RHIBufferDesc& desc)override;
		virtual RHIBuffer*			CreateVertexBuffer(const RHIVertexBufferDesc& desc) override;
		virtual RHIBuffer*			CreateIndexBuffer(const RHIIndexBufferDesc& desc) override;
		virtual RHIBuffer*			CreateUniformBuffer(const RHIUniformBufferDesc& desc)override;
		virtual RHITexture*			CreateTexture(const RHITextureDesc& desc) override;
		virtual RHITexture*			CreateTexture(const RHITextureDesc& desc, const void* data) override;

		virtual RHICommandList* CreateCommandList() override;

		virtual uint8_t	GetConcurrentFrameCount()const { return MAX_FRAMES_IN_FLIGHT; }
		virtual uint8_t GetCpuProcessFrameIndex()const override { return CURRENT_CPU_PROCESSES_FRAME_INDEX;}
		virtual uint8_t GetGpuProcessFrameIndex()const override { return m_bIsFirstRenderFrame ? CURRENT_CPU_PROCESSES_FRAME_INDEX : (CURRENT_CPU_PROCESSES_FRAME_INDEX + MAX_FRAMES_IN_FLIGHT - 1) % MAX_FRAMES_IN_FLIGHT; }


		VkInstance			GetvkInstance()				const { return m_vkInstance; }
		VkSurfaceKHR		GetvkSurface()				const { return m_vkSurface; }
		VkPhysicalDevice	GetvkPhysicalDevice()		const { return m_vkPhysicalDevice; }
		VkDevice			GetvkDevice()				const { return m_vkDevice; }
		VkQueue				GetvkGraphicsQueue()		const { return m_vkGraphicsQueue; }
		VkQueue				GetvkPresentQueue()			const { return m_vkPresentQueue; }
		uint32_t			GetvkGraphicsQueueFamily()	const { return m_vkGraphicsFamily; }
		uint32_t			GetvkPresentQueueFamily()	const { return m_vkPresentFamily; }
		VkSemaphore			GetRenderFinishedSemaphore()const { return m_RenderFinishedSemaphores[CURRENT_CPU_PROCESSES_FRAME_INDEX]; }
		VkSemaphore			GetImageAvailableSemaphore()const { return m_ImageAvailableSemaphores[CURRENT_CPU_PROCESSES_FRAME_INDEX]; }
		VkFence*			GetInFlightFence()			const { return const_cast<VkFence*>(&m_InFlightFences[CURRENT_CPU_PROCESSES_FRAME_INDEX]); }
		uint32_t			GetAPIVersion() 			const { return m_APIVersion; }


		uint32_t				FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
		SwapChainSupportDetails QueryvkSwapChainSupport(VkPhysicalDevice device)const;
		VkSurfaceFormatKHR		ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)const;
		VkSampleCountFlagBits	GetMaxUsableSampleCount()const;
		uint32_t				GetMaxPushConstantsSize()const;


		VkDescriptorSet					AllocateDescriptorSet(const VkDescriptorSetLayout* setLayout)const;
		std::vector<VkDescriptorSet>	AllocateDescriptorSets(const VkDescriptorSetLayout*setLayout,uint32_t count)const;
		void							FreeDescriptorSet(const VkDescriptorSet* set)const;
		void							FreeDescriptorSets(const VkDescriptorSet* sets,uint32_t count)const;

		/// <summary>
		/// 开始一个单次使用的命令缓冲区。
		/// </summary>
		/// <returns>一个用于单次命令记录的 VkCommandBuffer 对象。</returns>
		VkCommandBuffer BeginSingleTimeCommands();
		/// <summary>
		/// 结束单次命令的记录。
		/// </summary>
		void			EndSingleTimeCommands();


		//@todo use vma as alloactor
		void CreatevkBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)const;
		void CreatevkImage(VkImageType imageType, uint32_t width, uint32_t height, VkFormat format,VkSampleCountFlagBits sampleCount, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, uint32_t depth=1, uint32_t mipLevels=1, uint32_t arrayLayers=1)const;
		void CreatevkImageView(VkImage image, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspectMask, VkImageView& imageView, uint32_t baseMipLevel=0, uint32_t levelCount=1, uint32_t baseArrayLayer=0, uint32_t layerCount=1)const;
		
		void TransitionImageLayout(VkCommandBuffer cmdBuffer,VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, uint32_t baseMipLevel = 0, uint32_t levelCount = 1, uint32_t baseArrayLayer = 0, uint32_t layerCount = 1);
		void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask=VK_IMAGE_ASPECT_COLOR_BIT, uint32_t baseMipLevel=0, uint32_t levelCount=1, uint32_t baseArrayLayer=0, uint32_t layerCount=1);
		void CopyvkBufferTovkImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);





		VkFormat FindDepthFormat()const;
		/// <summary>
		/// 从候选格式列表中查找第一个支持指定功能的格式
		/// </summary>
		/// <param name="candidates">候选格式列表</param>
		/// <param name="tiling">期望的图像平铺方式</param>
		/// <param name="features">期望支持的功能标志</param>
		/// <returns></returns>
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)const;


	
	private:
		VkInstance					m_vkInstance		= VK_NULL_HANDLE; 
		VkDebugUtilsMessengerEXT	m_vkDebugMessenger	= VK_NULL_HANDLE; 
		VkSurfaceKHR				m_vkSurface			= VK_NULL_HANDLE; 
		VkPhysicalDevice			m_vkPhysicalDevice	= VK_NULL_HANDLE; 
		VkDevice					m_vkDevice			= VK_NULL_HANDLE; 
		VkDescriptorPool			m_vkDescriptorPool	= VK_NULL_HANDLE;

		VkQueue						m_vkGraphicsQueue	= VK_NULL_HANDLE; 
		VkQueue						m_vkPresentQueue	= VK_NULL_HANDLE; 
		uint32_t					m_vkGraphicsFamily	= 0;              
		uint32_t					m_vkPresentFamily	= 0;              
		void*						m_WindowHandle		= nullptr;        
		uint32_t					m_APIVersion{};

		VkCommandPool				m_SingleTimeCommandPool	= VK_NULL_HANDLE; // 专门用于一次性命令的池
		VkCommandBuffer				m_SingleTimeCommandBuffer	= VK_NULL_HANDLE;

		std::vector<VkSemaphore> m_RenderFinishedSemaphores;// 信号量：该图"渲染完了，可以呈现了"
		std::vector<VkSemaphore> m_ImageAvailableSemaphores;// 信号量：交换链"某张图可以取来画了"
		std::vector<VkFence>	m_InFlightFences;
	
		bool m_bIsFirstRenderFrame = true;

		void CreatevkInstance(bool enableValidation);  
	
		void SetupDebugMessenger();  
		
		void CreatevkSurface();   
		
		void PickPhysicalDevice();
	
		void CreatevkDevice();

		void CreateSingleTimevkCommandPool();

		void CreateSyncObjects();

		void CreatevkDescriptorPool();

		static bool CheckValidationLayerSupport(const std::vector<const char*>& layers);
		static std::vector<const char*> GetRequiredExtensions();
		/// <summary>
		/// 查找物理设备支持的队列族索引。
		/// </summary>
		/// <param name="device">要查询的物理设备 (VkPhysicalDevice)。</param>
		/// <param name="surface">用于检查队列支持的表面 (VkSurfaceKHR)。</param>
		/// <returns>包含队列族索引的 QueueFamilyIndices 对象。</returns>
		static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
		/// <summary>
		/// 检查指定的物理设备是否合适
		/// </summary>
		/// <param name="device">要检查的物理设备 (VkPhysicalDevice)。</param>
		/// <param name="surface">用于评估设备适用性的表面 (VkSurfaceKHR)。</param>
		/// <returns>如果设备适合使用给定的表面，则返回 true；否则返回 false。</returns>
		bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

		// 验证层消息回调：Vulkan 报错时会调它，把错误打印出来(开发期极有用)。
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT severity,
			VkDebugUtilsMessageTypeFlagsEXT type,
			const VkDebugUtilsMessengerCallbackDataEXT* data,
			void* userData);
	};

} // namespace DM::RHI
