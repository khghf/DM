#include<ImGuiBackend/ImGuiRenderer.h>
#include<Core/Renderer/Renderer.h>
#include<Engine.h>

#include<ImGuiBackend/ImGuiInitializer.h>
#include<backends/imgui_impl_glfw.h>
#ifdef  ENABLE_OPENGL_API
#include<backends/imgui_impl_opengl3.h>
#endif //  ENABLE_OPENGL_API


#ifdef ENABLE_VULKAN_API
#include<backends/imgui_impl_vulkan.h>
#include<Core/RHI/Backend/Vulkan/VulkanCommandList.h>
#endif // ENABLE_VULKAN_API
namespace DM
{
	using namespace RHI;
	

	ImGuiRenderer::ImGuiRenderer():
		m_SameMode(ESampleMode::x4), m_EnableDepth(true)
	{
		RHIDevice* device = RHIDevice::Get();
		Renderer* engineRenderer = Engine::Get()->GetRenderer();
		auto engineSwapchain = engineRenderer->GetSwapchain();


		EFormat format = engineRenderer->GetSwapchain()->GetFormat();


		std::vector<AttachmentDesc>attachmentDesc{};

		attachmentDesc.emplace_back(format, ERHIAttachmentUsage::ColorTarget);
		attachmentDesc.emplace_back(EFormat::Unknown, ERHIAttachmentUsage::DepthTarget);//创建深度附件时会自动选择合适的格式


		RHIRenderPassDesc renderDesc{};
		renderDesc.EnableDepth = m_EnableDepth;
		renderDesc.Format = format;
		renderDesc.SampleMode = m_SameMode;
		renderDesc.Attachments = attachmentDesc;
		m_RenderPass = device->CreateRenderPass(renderDesc);

		m_Framebuffers.reserve(engineSwapchain->GetImageCount());
		RHIFramebufferDesc framebufferDesc{};
		framebufferDesc.Attachments = attachmentDesc;
		framebufferDesc.SampleMode = m_SameMode;
		engineSwapchain->GetExtent(framebufferDesc.Width, framebufferDesc.Height);

		for (int i = 0; i < m_Framebuffers.capacity(); ++i)
		{
			m_Framebuffers.emplace_back(m_RenderPass->CreateFramebuffer(framebufferDesc));
		}
	}
	ImGuiRenderer::~ImGuiRenderer()
	{
		for (auto in : m_Framebuffers)delete in;
		delete m_RenderPass;
	}

	void ImGuiRenderer::Begin()
	{
		ImGui_ImplGlfw_NewFrame();
#if defined(ENABLE_VULKAN_API)
		ImGui_ImplVulkan_NewFrame();
#elif defined(ENABLE_OPENGL_API)
		ImGui_ImplOpenGL3_NewFrame();
#else
#error "No graphics API backend enabled for ImGui!"
#endif
		ImGui::NewFrame();
		BeginDockSpace();
		Renderer* engineRenderer = Engine::Get()->GetRenderer();
		auto engineSwapchain = engineRenderer->GetSwapchain();
		m_AvailableFramebufferIndex = engineSwapchain->GetCurrentImageIndex();
		auto framebuffer = GetAvailableFramebuffer();
		uint32_t width{}, height{};
		engineSwapchain->GetExtent(width, height);
		if (framebuffer->GetWidth() != width || framebuffer->GetHeight() != height)
		{
			framebuffer->Resize(width, height);
		}
	}

	void ImGuiRenderer::End()
	{
		EndDockSpace();
		ImGui::Render();
#ifdef ENABLE_VULKAN_API
		Renderer* engineRenderer = Engine::Get()->GetRenderer();
		RHI::RHICommandList* cmd = engineRenderer->GetCommandList();
		engineRenderer->GetSwapchain()->SetPresentTaregt(GetAvailableFramebuffer());
		cmd->BeginRenderPass(m_RenderPass, GetAvailableFramebuffer());
		VkCommandBuffer cmdBuffer = *(static_cast<RHI::VulkanCommandList*>(cmd)->GetIdleCommandBuffer());
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);
		cmd->EndRenderPass();
#endif

#ifdef ENABLE_OPENGL_API
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
	}



	void ImGuiRenderer::BeginDockSpace()
	{
		ImGuiIO& io = ImGui::GetIO();
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)window_flags |= ImGuiWindowFlags_NoBackground;
		if (!opt_padding)ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("DockSpace Demo", nullptr, window_flags);

		if (!opt_padding)ImGui::PopStyleVar();
		if (opt_fullscreen)ImGui::PopStyleVar(2);
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
	}
	void ImGuiRenderer::EndDockSpace()
	{
		ImGui::End();
	}

	RHI::RHIFramebuffer* ImGuiRenderer::GetAvailableFramebuffer() const
	{
		return m_Framebuffers[m_AvailableFramebufferIndex];
	}
}