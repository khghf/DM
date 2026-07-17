#pragma once
#include"RHITypes.h"
#include<Core/MMM/Reference.h>
#include<Core/Math/Vector.h>

namespace DM
{
	// 前向声明现有接口类
	class VertexArray;
	class VertexBuffer;
	class IndexBuffer;
	class Shader;
	class Texture2D;
	class FrameBuffer;
	class UniformBuffer;
	class RendererAPI;
	struct FrameBufferSpecification;
}

namespace DM::RHI
{
	// ============================================================
	// RHIDevice - 集中式设备管理 & 资源工厂
	//
	// 职责:
	//   1. 管理底层 RendererAPI 实例 (OpenGL / future Vulkan / DX12)
	//   2. 提供所有 RHI 资源的统一创建入口
	//   3. 隔离顶层渲染代码与平台 #include 依赖
	// ============================================================
	class DM_API RHIDevice
	{
	public:
		static RHIDevice& Get();

		void Init();
		void Shutdown();

		// ---- 驱动查询 ----
		EDriver GetDriver() const { return m_Driver; }

		// ---- 视口 / 清屏 / 提交 ----
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		void SetClearColor(const Vector4& color);
		void Clear();
		void DrawIndexed(const SPtr<VertexArray>& vertexArray, uint32_t indexCount = 0);

		// ---- 资源工厂 (替代各接口类中散落的 static Create) ----
		SPtr<VertexBuffer> CreateVertexBuffer(uint32_t sizeBytes);
		SPtr<VertexBuffer> CreateVertexBuffer(float* vertices, uint32_t count);
		SPtr<IndexBuffer>  CreateIndexBuffer(uint32_t* indices, uint32_t count);
		SPtr<VertexArray>  CreateVertexArray();
		SPtr<Shader>       CreateShader(const std::string_view& name, const std::string_view& vsSrc, const std::string_view& fsSrc);
		SPtr<Shader>       CreateShader(const std::string_view& filepath);
		SPtr<Texture2D>    CreateTexture2D(const std::string_view& filepath);
		SPtr<Texture2D>    CreateTexture2D(uint32_t width, uint32_t height);
		SPtr<FrameBuffer>  CreateFrameBuffer(const FrameBufferSpecification& spec);
		SPtr<UniformBuffer> CreateUniformBuffer(uint32_t size, uint32_t binding);

	private:
		RHIDevice() = default;
		~RHIDevice();
		RHIDevice(const RHIDevice&) = delete;
		RHIDevice& operator=(const RHIDevice&) = delete;

		EDriver m_Driver = EDriver::OpenGL;
		RendererAPI* m_ApiImpl = nullptr;           // 平台 API 实例 (OpenGlRendererAPI)
		bool m_bInitialized = false;
	};

} // namespace DM::RHI
