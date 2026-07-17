#include"DMPCH.h"
#include"Core/RHI/RHIDevice.h"

// ============================================================
// 平台驱动集中包含点 - 所有 OpenGL 依赖限定于此文件
// 将来切换 Vulkan/DX12 时仅需修改此处
// ============================================================
#include"Core/Render/Renderer/RendererAPI.h"
#include<Platform/Render/OpenGl/Renderer/OpenGlRendererAPI.h>
#include<Platform/Render/OpenGl/Buffer/OpenGlBuffer.h>
#include<Platform/Render/OpenGl/OpenGlVertexArray.h>
#include<Platform/Render/OpenGl/OpenGlShader.h>
#include<Platform/Render/OpenGl/Texture/OpenGlTexture.h>
#include<Platform/Render/OpenGl/FrameBuffer/OpenGlFrameBuffer.h>
#include<Platform/Render/OpenGl/UniformBuffer/OpenGlUniformBuffer.h>

#include<Core/Render/Buffer.h>
#include<Core/Render/VertexArray.h>
#include<Core/Render/Shader.h>
#include<Core/Render/Texture/Texture.h>
#include<Core/Render/FrameBuffer.h>
#include<Core/Render/UniformBuffer.h>
#include<Core/Log.h>

namespace DM::RHI
{
	RHIDevice& RHIDevice::Get()
	{
		static RHIDevice instance;
		return instance;
	}

	RHIDevice::~RHIDevice()
	{
		delete m_ApiImpl;
		m_ApiImpl = nullptr;
	}

	void RHIDevice::Init()
	{
		if (m_bInitialized) return;
		m_ApiImpl = new OpenGlRendererAPI();
		m_ApiImpl->Init();
		m_bInitialized = true;
		LOG_CORE_INFO("RHIDevice initialized with OpenGL driver");
	}

	void RHIDevice::Shutdown()
	{
		if (!m_bInitialized) return;
		delete m_ApiImpl;
		m_ApiImpl = nullptr;
		m_bInitialized = false;
	}

	// ---- RHI 指令 ----
	void RHIDevice::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		if (!m_ApiImpl) { Init(); }
		m_ApiImpl->SetViewport(x, y, width, height);
	}

	void RHIDevice::SetClearColor(const Vector4& color)
	{
		if (!m_ApiImpl) { Init(); }
		m_ApiImpl->SetClearColor(color);
	}

	void RHIDevice::Clear()
	{
		if (!m_ApiImpl) { Init(); }
		m_ApiImpl->Clear();
	}

	void RHIDevice::DrawIndexed(const SPtr<VertexArray>& vertexArray, uint32_t indexCount)
	{
		if (!m_ApiImpl) { Init(); }
		m_ApiImpl->DrawIndexed(vertexArray, indexCount);
	}

	// ---- 资源工厂 ----
	SPtr<VertexBuffer> RHIDevice::CreateVertexBuffer(uint32_t sizeBytes)
	{
		return std::make_shared<OpenGlVertexBuffer>(sizeBytes);
	}

	SPtr<VertexBuffer> RHIDevice::CreateVertexBuffer(float* vertices, uint32_t count)
	{
		return std::make_shared<OpenGlVertexBuffer>(vertices, count);
	}

	SPtr<IndexBuffer> RHIDevice::CreateIndexBuffer(uint32_t* indices, uint32_t count)
	{
		return std::make_shared<OpenGlIndexBuffer>(indices, count);
	}

	SPtr<VertexArray> RHIDevice::CreateVertexArray()
	{
		return std::make_shared<OpenGlVertexArray>();
	}

	SPtr<Shader> RHIDevice::CreateShader(const std::string_view& name, const std::string_view& vsSrc, const std::string_view& fsSrc)
	{
		return std::make_shared<OpenGlShader>(name, vsSrc, fsSrc);
	}

	SPtr<Shader> RHIDevice::CreateShader(const std::string_view& filepath)
	{
		return std::make_shared<OpenGlShader>(filepath);
	}

	SPtr<Texture2D> RHIDevice::CreateTexture2D(const std::string_view& filepath)
	{
		return CreateSPtr<OpenGlTexture2D>(filepath);
	}

	SPtr<Texture2D> RHIDevice::CreateTexture2D(uint32_t width, uint32_t height)
	{
		return CreateSPtr<OpenGlTexture2D>(width, height);
	}

	SPtr<FrameBuffer> RHIDevice::CreateFrameBuffer(const FrameBufferSpecification& spec)
	{
		return CreateSPtr<OpenGlFrameBuffer>(spec);
	}

	SPtr<UniformBuffer> RHIDevice::CreateUniformBuffer(uint32_t size, uint32_t binding)
	{
		return CreateSPtr<OpenGlUniformBuffer>(size, binding);
	}

} // namespace DM::RHI
