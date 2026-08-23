#include<Core/Renderer/TriangleRenderer.h>
#include<Core/RHI/RHI.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<Core/Log.h>
#include<Core/AssetManagent/AssetMgr.h>
#include<Framework/Component/TransformComponent.h>
#include<Framework/Component/SpriteComponent.h>
#include<Foundation/GameStatic.h>
namespace DM
{
	using namespace DM::RHI;
	static uint32_t GenGeometryInstanceId() { static uint32_t id{}; return id++; }

	void TriangleRenderer::UpdateGlobalData(int index)
	{
		auto& data = m_GlobalBuffers[index];

		GlobalData newData{};

		newData.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		newData.Proj= glm::perspective(glm::radians(45.0f), 1280.f/720, 0.1f, 10.0f);

		data->Update(&newData, sizeof(GlobalData));
	}

	void TriangleRenderer::UpdateInstanceData(int index)
	{

		static auto startTime = std::chrono::high_resolution_clock::now();


		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		std::vector<InstanceData>newData(2);
		int i = 1;
		for (auto& ele : newData)
		{
			ele.Model = glm::translate(ele.Model, { 0.f,0.f,glm::abs(glm::sin(time/4*3.14))*i});
			ele.Model = ele.Model * glm::rotate(glm::mat4{1.f}, time * glm::radians(90.0f*i), glm::vec3(0.0f, 0.0f, 1.0f));
			i *= -1;
		}

		//auto& data = m_InstanceBuffers[index];

		//data->Update(newData.data(),newData.size()*sizeof(InstanceData));
	}

	TriangleRenderer::TriangleRenderer()
		:m_SameMode(ESampleMode::x4),m_EnableDepth(true)
	{

		RHIDevice* device = RHIDevice::Get();



		RHISwapchainDesc swapDesc{};
		swapDesc.WindowHandle = device->GetWindowHandle();
		swapDesc.SampleMode = m_SameMode;
		m_Swapchain =  device->CreateSwapchain(swapDesc);



		std::vector<AttachmentDesc>attachmentDesc{};
		attachmentDesc.emplace_back(m_Swapchain->GetFormat(), ERHIAttachmentUsage::ColorTarget);
		attachmentDesc.emplace_back(EFormat::Unknown, ERHIAttachmentUsage::DepthTarget);//创建深度附件时会自动选择合适的格式
		RHIRenderPassDesc renderDesc{};
		renderDesc.EnableDepth = m_EnableDepth;
		renderDesc.Format = m_Swapchain->GetFormat();
		renderDesc.SampleMode = m_SameMode;
		renderDesc.Attachments = attachmentDesc;
		m_RenderPass = device->CreateRenderPass(renderDesc);



		SPtr<DM::Shader>vs = DM::AssetMgr::LoadAsset<DM::Shader>("Assets/Shader/BuiltIn.vert");
		SPtr<DM::Shader>fs = DM::AssetMgr::LoadAsset<DM::Shader>("Assets/Shader/BuiltIn.frag");



		RHIShaderProgramDesc shaderProgramDesc{};
		shaderProgramDesc.Shaders.push_back(vs->GetRHIResource());
		shaderProgramDesc.Shaders.push_back(fs->GetRHIResource());
		m_ShaderProgram = device->CreateShaderProgram(shaderProgramDesc);



		RHIPipelineDesc pipeDesc{};
		pipeDesc.Topology = EPrimitiveTopology::TriangleList;
		pipeDesc.CullMode = ECullMode::Back;
		pipeDesc.FillMode = EPolygonMode::Fill;
		pipeDesc.DepthTest = true;
		pipeDesc.DepthWrite = true;
		pipeDesc.ShaderProgram = m_ShaderProgram;
		pipeDesc.RenderPass = m_RenderPass;
		m_Pipeline = device->CreatePipeline(pipeDesc);


		m_Cmd = device->CreateCommandList();


	/*	VertexSet set = 
		{
				{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
				{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
				{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
				{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
		};

		VertexLayout layout = {
			{"inPos",EFormat::Float3},
			{"inColor",EFormat::Float3},
			{"inTexCoord",EFormat::Float2},
		};*/



		/*RHIVertexBufferDesc vertexbufferDesc{ set, layout };
		m_VertexBuffer = device->CreateVertexBuffer(vertexbufferDesc);


		std::vector<uint32_t>indices{ 0, 1, 2, 2, 3, 0};
		m_IndexCount = indices.size();
		RHIIndexBufferDesc indexBufferDesc{ indices };
		m_IndexBuffer =  device->CreateIndexBuffer(indexBufferDesc);*/


		RHIBufferDesc bufferDesc{};
		bufferDesc.SizeBytes = sizeof(GlobalData);
		bufferDesc.Type = EBufferType::UniformBuffer;
		m_GlobalBuffers.resize(device->GetConcurrentFrameCount());
		for(auto&ele:m_GlobalBuffers)ele= device->CreateBuffer(bufferDesc);


	/*	RHIUniformBufferDesc instanceBufferDesc{};
		instanceBufferDesc.Type = EBufferType::StorageBuffer;
		instanceBufferDesc.m_SizeBytes = sizeof(InstanceData)*MAX_INSTANCE_COUNT;
		m_InstanceBuffers.resize(device->GetConcurrentFrameCount());
		for (auto& ele : m_InstanceBuffers)ele = device->CreateUniformBuffer(instanceBufferDesc);*/


		m_Framebuffers.resize(m_Swapchain->GetImageCount());
		RHIFramebufferDesc framebufferDesc{};
		framebufferDesc.Attachments = attachmentDesc;
		framebufferDesc.SampleMode = m_SameMode;
		m_Swapchain->GetExtent(framebufferDesc.Width, framebufferDesc.Height);
		for (auto& ele : m_Framebuffers)ele = m_RenderPass->CreateFramebuffer(framebufferDesc);


		m_GlobalSets.resize(device->GetConcurrentFrameCount());
		for (auto& ele : m_GlobalSets)ele = m_ShaderProgram->GenDescriptorSet(0);


		for (int i=0;i< m_GlobalSets.size();++i)m_GlobalSets[i]->BindUBO(m_GlobalBuffers[i],"globalData");

	}

	TriangleRenderer::~TriangleRenderer()
	{

		RHIDevice::Get()->WaitGPUIdle();
		
		for (auto& batchs : m_InstanceBatchs)
		{
			for (auto& inst : batchs.second)delete inst;
		}

		std::ranges::for_each(m_GlobalSets, [](const auto* in) {delete in; });
		std::ranges::for_each(m_GlobalBuffers, [](const auto* in) {delete in; });
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

		m_Swapchain->SetPresentTaregt(GetAvailableFramebuffer());

		m_Cmd->BeginRenderPass(m_RenderPass, m_Framebuffers[m_AvailableFramebufferIndex]);
		m_Cmd->BindPipeline(m_Pipeline);
		m_Cmd->BindDescriptorSet(m_ShaderProgram, m_GlobalSets[RHIDevice::Get()->GetCpuProcessFrameIndex()]);
		m_Cmd->SetViewport(width, height);

	}

	void TriangleRenderer::Update(float deltaTime)
	{
		static uint32_t frameIndex{};
		frameIndex = RHIDevice::Get()->GetCpuProcessFrameIndex();
		UpdateGlobalData(frameIndex);

		UpdateRenderData();

		FlushForce();
		m_Cmd->EndRenderPass();
	}

	void TriangleRenderer::EndFrame()
	{
		m_Cmd->EndRecord();
		m_Cmd->Submit();
		m_Swapchain->Present();
		Renderer::EndFrame();
	}
	
	void TriangleRenderer::Flush()
	{
		for (const auto& in : m_InstanceBatchs)
		{
			if (in.second.size() <= 0)continue;
			const auto& instance = in.second[RHIDevice::Get()->GetCpuProcessFrameIndex()];

			if (instance&& instance->Instances.size()==MAX_INSTANCE_COUNT)
			{
				m_Cmd->BindVertexBuffer(instance->VertexBuffer);
				m_Cmd->BindIndexBuffer(instance->IndexBuffer);
				instance->InstanceBuffer->Update(instance->Instances.data(), instance->Instances.size() * sizeof(InstanceData));


				m_Cmd->BindDescriptorSet(m_ShaderProgram, instance->DescriptorSet);
				m_Cmd->DrawIndex(instance->IndexCount, instance->Instances.size());
				instance->Instances.clear();
			}
		}
	}

	void TriangleRenderer::FlushForce()
	{
		for (const auto& in : m_InstanceBatchs)
		{
			if (in.second.size() <= 0)continue;
			const auto& instance = in.second[RHIDevice::Get()->GetCpuProcessFrameIndex()];

			if (instance && instance->Instances.size()>0)
			{
				m_Cmd->BindVertexBuffer(instance->VertexBuffer);
				m_Cmd->BindIndexBuffer(instance->IndexBuffer);
				instance->InstanceBuffer->Update(instance->Instances.data(), instance->Instances.size() * sizeof(InstanceData));
				m_Cmd->BindDescriptorSet(m_ShaderProgram, instance->DescriptorSet);
				m_Cmd->DrawIndex(instance->IndexCount, instance->Instances.size());
				instance->Instances.clear();
			}
		}
	}

	RHI::RHIFramebuffer* TriangleRenderer::GetAvailableFramebuffer() const
	{
		return m_Framebuffers[m_AvailableFramebufferIndex];
	}

	void TriangleRenderer::UpdateRenderData()
	{
		auto world = GameStatic::GetCurWorld();
		if (world.expired()) return;

		const auto& registry = world.lock()->GetEnttRegistry();
		auto view = registry.view<SpriteComponent, TransformComponent>();

		// 清空渲染数据
		m_RenderData.Clear();

		// 遍历所有实体
		for (const auto& [entity, sprite, transform] : view.each())
		{
			Matrix4 model = transform.GetTransform();
			DrawQuad(model, sprite.Color);
		}
	}


	InstanceBatch* TriangleRenderer::CreateInstanceBatch()
	{
		InstanceBatch* result = new InstanceBatch();
		result->DescriptorSet = m_ShaderProgram->GenDescriptorSet(1);
		result->Instances.reserve(MAX_INSTANCE_BYTES / sizeof(InstanceData));
		return result;
	}

	

	void TriangleRenderer::DrawQuad(const Matrix4& transform, const Vector4& color)
	{
		static uint32_t id{};

		static const std::vector<float> quadVertices = 
		{
		   -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
			0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
			0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
		   -0.5f,  0.5f, 0.0f,   0.0f, 1.0f
		};

		static const std::vector<uint32_t> quadIndices = { 0, 1, 2, 2, 3, 0 };

		// 批次只创建一次
		static bool batchCreated = false;
		if (!batchCreated)
		{
			id=GenGeometryInstanceId();
			RHI::RHIDevice* device = RHI::RHIDevice::Get();
			auto& instanceBatchs = m_InstanceBatchs[id];  // 四边形批次 key = 0
			instanceBatchs.resize(device->GetConcurrentFrameCount());

			for (auto& instanceBatch : instanceBatchs)
			{
				instanceBatch = CreateInstanceBatch();

				RHIBufferDesc bufferDesc{};
				bufferDesc.SizeBytes = quadVertices.size() * sizeof(float);
				bufferDesc.Type = EBufferType::VertexBuffer;
				instanceBatch->VertexBuffer = device->CreateBuffer(bufferDesc);
				instanceBatch->VertexBuffer->Update(quadVertices.data(),quadVertices.size() * sizeof(float));

				bufferDesc.SizeBytes = quadIndices.size() * sizeof(uint32_t);
				bufferDesc.Type = EBufferType::IndexBuffer;
				instanceBatch->IndexBuffer = device->CreateBuffer(bufferDesc);
				instanceBatch->IndexBuffer->Update(quadIndices.data(),quadIndices.size() * sizeof(uint32_t));

				bufferDesc.SizeBytes = MAX_INSTANCE_BYTES;
				bufferDesc.Type = EBufferType::StorageBuffer;
				instanceBatch->InstanceBuffer = device->CreateBuffer(bufferDesc);

				instanceBatch->DescriptorSet->BindUBO(instanceBatch->InstanceBuffer, "instanceData");
				instanceBatch->IndexCount = quadIndices.size();
			}
			batchCreated = true;
		}

		uint32_t frameIndex = RHI::RHIDevice::Get()->GetCpuProcessFrameIndex();
		auto& instances = m_InstanceBatchs[id][frameIndex]->Instances;

		if (instances.size() >= MAX_INSTANCE_COUNT)Flush();

		InstanceData data;
		data.Model = transform;
		data.Color = color;
		instances.emplace_back(data);

	}
}
