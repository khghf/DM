#include<Editor/UI/Backend/ImGuiInitializer.h>
#include<Core/RHI/RHI.h>
#include<imgui.h>
#include<backends/imgui_impl_glfw.h>

#ifdef  ENABLE_OPENGL_API
#include<backends/imgui_impl_opengl3.h>
#endif //  ENABLE_OPENGL_API


#ifdef ENABLE_VULKAN_API
#include<backends/imgui_impl_vulkan.h>
#include<Core/RHI/Backend/Vulkan/VulkanDevice.h>
#include<Core/RHI/Backend/Vulkan/VulkanSwapchain.h>
#include<Core/RHI/Backend/Vulkan/VulkanRenderPass.h>
#endif // ENABLE_VULKAN_API

#include<Engine.h>
#include<Editor/UI/Backend/ImGuiRenderer.h>
#include<Editor/UI/Style/EditorStyle.h>
namespace DM
{
	void ImGuiInitializer::Init(ImGuiRenderer* imGuiRenderer)
	{
		ImGui::CreateContext();
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

#ifdef  ENABLE_OPENGL_API
		ImGui_ImplOpenGL3_Init("#version 460");
		ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(Engine::Get()->GetAppWindow().GetNativeWindow()), true);
#endif
#ifdef ENABLE_VULKAN_API
		ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow*>(Engine::Get()->GetAppWindow().GetNativeWindow()), true);
		ImGui_ImplVulkan_InitInfo init_info = {};

		Renderer* engineRenderer = Engine::GetRenderer();

		RHI::VulkanDevice* device = static_cast<RHI::VulkanDevice*>(RHI::GetDevice());
		RHI::VulkanSwapchain* engineSwapchain = static_cast<RHI::VulkanSwapchain*>(engineRenderer->GetSwapchain());
		RHI::VulkanRenderPass* ImGuiRenderPass = static_cast<RHI::VulkanRenderPass*>(imGuiRenderer->m_RenderPass);


		init_info.ApiVersion = device->GetAPIVersion();              
		init_info.Instance = device->GetvkInstance();
		init_info.PhysicalDevice = device->GetvkPhysicalDevice();
		init_info.Device = device->GetvkDevice();
		init_info.QueueFamily = device->GetvkGraphicsQueueFamily();
		init_info.Queue = device->GetvkGraphicsQueue();
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPoolSize = 2000;
		init_info.MinImageCount = engineSwapchain->GetMinImageCount();
		init_info.ImageCount = engineSwapchain->GetImageCount();
		init_info.Allocator = nullptr;
		init_info.PipelineInfoMain.RenderPass = ImGuiRenderPass->GetvkRenderPass();//为imgui额外创建的renderpass以实现离屏渲染
		init_info.PipelineInfoMain.Subpass = 0;
		init_info.PipelineInfoMain.MSAASamples = engineSwapchain->GetvkSampleCount();
		init_info.CheckVkResultFn = [](VkResult err) {VK_CHECK(err); };
		ImGui_ImplVulkan_Init(&init_info);
#endif

		EditorStyle::SetDefaultStyle();   // 内部已通过 GetConfigPath() 加载 Config/Style.json

		EditorStyle::ApplyCurrentStyle();
	}
	void ImGuiInitializer::ShutDown()
	{
		ImGui_ImplGlfw_Shutdown();
#ifdef  ENABLE_OPENGL_API
		ImGui_ImplOpenGL3_Shutdown();
#endif
#ifdef ENABLE_VULKAN_API
		ImGui_ImplVulkan_Shutdown();
#endif
	}
}