#pragma once
#include"Core/RHI/RHITypes.h"
namespace DM::RHI
{
	class RHIRenderPass;
	class RHISwapchain;
	class RHIBuffer;
	class RHIShader;
	class RHIShaderProgram;
	class RHIPipeline;
	class RHICommandList;
	class RHITexture;
	class DM_API RHIDevice
	{
	public:
		static void Init(const RHIDeviceDesc& desc);
		static void ShutDown();
		static RHIDevice* Get() { return m_Inst; };

		virtual ~RHIDevice()=default;
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		/// <summary>
		/// 阻塞主线程直到GPU空闲下来(这时GPU没有引用任何资源可以在之后安全释放申请的资源)
		/// </summary>
		virtual void WaitGPUIdle() = 0;

		virtual RHISwapchain*		CreateSwapchain(const RHISwapchainDesc& desc) = 0;
		virtual RHIRenderPass*		CreateRenderPass(const RHIRenderPassDesc& desc) = 0;
		virtual RHIShader*			CreateShader(const RHIShaderDesc& desc) = 0;
		virtual RHIShaderProgram*	CreateShaderProgram(const RHIShaderProgramDesc& desc) =0;
		virtual RHIPipeline*		CreatePipeline(const RHIPipelineDesc& desc) = 0;

		virtual RHIBuffer*			CreateBuffer(const RHIBufferDesc& desc) = 0;
		virtual RHIBuffer*			CreateVertexBuffer(const RHIVertexBufferDesc& desc) = 0;
		virtual RHIBuffer*			CreateIndexBuffer(const RHIIndexBufferDesc& desc) = 0;
		virtual RHIBuffer*			CreateUniformBuffer(const RHIUniformBufferDesc& desc) = 0;
		virtual RHITexture*			CreateTexture(const RHITextureDesc& desc) = 0;
		virtual RHITexture*			CreateTexture(const RHITextureDesc& desc,const void*data) = 0;
		virtual RHICommandList*		CreateCommandList() = 0;

		void* GetWindowHandle()const { return m_WindowHandle; }


		virtual uint8_t	GetConcurrentFrameCount()const { return 1; }
		/// <summary>
		/// 获取CPU当前正在渲染的帧的索引。
		/// </summary>
		/// <returns>当前 CPU 处理帧的索引，返回值为一个 8 位无符号整数。</returns>
		virtual uint8_t GetCpuProcessFrameIndex()const { return 0; }
		/// <summary>
		/// 获取GPU当前正在处理的帧的索引。
		/// </summary>
		/// <returns>GPU 进程的帧索引，返回值类型为 uint8_t。</returns>
		virtual uint8_t GetGpuProcessFrameIndex()const { return 0; }



	protected:
		RHIDevice() = default;
		static RHIDevice* m_Inst;
		void* m_WindowHandle;
	};

} // namespace DM::RHI
