#include<Core/Renderer/TriangleRenderer.h>
#include<Core/RHI/RHI.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<Core/Log.h>
#include<Core/AssetManagent/AssetMgr.h>

namespace DM
{
	using namespace DM::RHI;
	struct alignas(16) UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	void UpdateUniformBuffer(RHIBuffer* buffer, RHISwapchain* swapchain)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		uint32_t width, height;
		swapchain->GetExtent(width, height);
		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), width / (float)height, 0.1f, 10.0f);
		buffer->UpdateData(&ubo, sizeof(ubo));
	}


	TriangleRenderer::TriangleRenderer():
		m_SameMode(ESampleMode::x4),m_EnableDepth(true)
	{

		RHIDevice* device = RHIDevice::Get();

		RHISwapchainDesc swapDesc{};
		swapDesc.WindowHandle = device->GetWindowHandle();
		swapDesc.SampleMode = m_SameMode;
		m_Swapchain =  device->CreateSwapchain(swapDesc);


		std::vector<AttachmentDesc>attachmentDesc{};

		attachmentDesc.emplace_back(m_Swapchain->GetFormat(), ERHIAttachmentUsage::ColorTarget);
		attachmentDesc.emplace_back(ERHIFormat::Unknown, ERHIAttachmentUsage::DepthTarget);//创建深度附件时会自动选择合适的格式


		RHIRenderPassDesc renderDesc{};
		renderDesc.EnableDepth = m_EnableDepth;
		renderDesc.Format = m_Swapchain->GetFormat();
		renderDesc.SampleMode = m_SameMode;
		renderDesc.Attachments = attachmentDesc;
		m_RenderPass = device->CreateRenderPass(renderDesc);


		m_Framebuffers.reserve(m_Swapchain->GetImageCount());
		RHIFramebufferDesc framebufferDesc{};
		framebufferDesc.Attachments = attachmentDesc;
		framebufferDesc.SampleMode = m_SameMode;
		m_Swapchain->GetExtent(framebufferDesc.Width, framebufferDesc.Height);

		for (int i = 0; i < m_Framebuffers.capacity(); ++i)
		{
			m_Framebuffers.emplace_back(m_RenderPass->CreateFramebuffer(framebufferDesc));
		}

		SPtr<DM::Shader>vs = DM::AssetMgr::LoadAsset<DM::Shader>("Assets/Shader/Texture.vs");
		SPtr<DM::Shader>fs = DM::AssetMgr::LoadAsset<DM::Shader>("Assets/Shader/Texture.fs");



		VertexSet set = {
				{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
				{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
				{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
				{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

				{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
				{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
				{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
				{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
		};
		VertexLayout layout = {
			{"inPos",ERHIFormat::Float3},
			{"inColor",ERHIFormat::Float3},
			{"inTexCoord",ERHIFormat::Float2},
		};



		RHIVertexBufferDesc vertexbufferDesc{ set, layout };

		std::vector<uint32_t>indices{ 0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4 };
		m_IndexCount = indices.size();
		RHIIndexBufferDesc indexBufferDesc{ indices };

		RHIUniformBufferDesc unifromBufferDesc{};
		unifromBufferDesc.m_SizeBytes = sizeof(UniformBufferObject);

		m_Vertex = device->CreateVertexBuffer(vertexbufferDesc);
		m_Index =  device->CreateIndexBuffer(indexBufferDesc);

		m_Uniform.resize(2);
		m_Uniform[0] = device->CreateUniformBuffer(unifromBufferDesc);
		m_Uniform[1] = device->CreateUniformBuffer(unifromBufferDesc);


		RHIShaderProgramDesc shaderProgramDesc{};
		shaderProgramDesc.Shaders.push_back(vs->GetRHIResource());
		shaderProgramDesc.Shaders.push_back(fs->GetRHIResource());
		m_ShaderProgram = device->CreateShaderProgram(shaderProgramDesc);


		m_SetGroups.push_back(m_ShaderProgram->GenDescriptorSetGroup());
		m_SetGroups.push_back(m_ShaderProgram->GenDescriptorSetGroup());

		m_Texture = DM::AssetMgr::LoadAsset<DM::Texture2D>("Assets/Texture/Container/container_diffuse.png");

		m_SetGroups[0]->BindUBO(m_Uniform[0], "ubo");
		m_SetGroups[0]->BindTex(m_Texture->GetRHIResource(), "texSampler");


		m_SetGroups[1]->BindUBO(m_Uniform[1], "ubo");
		m_SetGroups[1]->BindTex(m_Texture->GetRHIResource(), "texSampler");

		RHIPipelineDesc pipeDesc{};

		pipeDesc.ShaderProgram = m_ShaderProgram;
		pipeDesc.Topology = EPrimitiveTopology::TriangleList; 
		pipeDesc.CullMode = ECullMode::Back;
		pipeDesc.FillMode = EPolygonMode::Fill;
		pipeDesc.DepthTest = true;
		pipeDesc.DepthWrite = true;
		pipeDesc.RenderPass = m_RenderPass;


		m_Pipeline = device->CreatePipeline(pipeDesc);

		m_Cmd = device->CreateCommandList();
	}

	TriangleRenderer::~TriangleRenderer()
	{

		RHIDevice::Get()->WaitGPUIdle();
		delete m_Vertex;
		delete m_Index;

		std::ranges::for_each(m_Uniform, [](const auto* in) {delete in; });
		m_Texture.reset();
		std::ranges::for_each(m_SetGroups, [](const auto* in) {delete in; });
		std::ranges::for_each(m_Framebuffers, [](const auto* in) {delete in; });

		delete m_Cmd;
		delete m_ShaderProgram;

		delete m_Pipeline;
		delete m_Swapchain;
		delete m_RenderPass;

	}

	void TriangleRenderer::BeginFrame()
	{
		Renderer::BeginFrame();

		m_AvailableFramebufferIndex=m_Swapchain->AcquireNextImage();

		auto framebuffer = GetAvailableFramebuffer();

		uint32_t width{}, height{};
		m_Swapchain->GetExtent(width, height);
		if (framebuffer->GetWidth() != width || framebuffer->GetHeight() != height)
		{
			framebuffer->Resize(width, height);
		}
		m_Cmd->BeginRecord();
	}
	void TriangleRenderer::UpdateData(float deltaTime)
	{
		static uint32_t frameIndex{};
		frameIndex= RHIDevice::Get()->GetGpuProcessFrameIndex();

		UpdateUniformBuffer(m_Uniform[frameIndex], m_Swapchain);

		uint32_t width{}, height{};
		m_Swapchain->GetExtent(width, height);

		m_Swapchain->SetPresentTaregt(GetAvailableFramebuffer());//最后呈现时会将数据拷贝到交换链图像

		m_Cmd->BeginRenderPass(m_RenderPass, m_Framebuffers[m_AvailableFramebufferIndex]);
		m_Cmd->BindPipeline(m_Pipeline);

		m_Cmd->BindDescriptorSetGroup(m_ShaderProgram, m_SetGroups[frameIndex]);

		m_Cmd->BindVertexBuffer(m_Vertex);
		m_Cmd->BindIndexBuffer(m_Index);
		m_Cmd->SetViewport(width, height);
		m_Cmd->DrawIndex(m_IndexCount);
		m_Cmd->EndRenderPass();
	}
	void TriangleRenderer::EndFrame()
	{
		m_Cmd->EndRecord();
		m_Cmd->Submit();
		m_Swapchain->Present();
		Renderer::EndFrame();
	}
	RHI::RHIFramebuffer* TriangleRenderer::GetAvailableFramebuffer() const
	{
		return m_Framebuffers[m_AvailableFramebufferIndex];
	}
}
