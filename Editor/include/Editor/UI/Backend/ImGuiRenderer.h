#pragma once
#include<Core/RHI/RHI.h>
namespace DM
{
	class ImGuiRenderer
	{
		friend class ImGuiInitializer;
	private:
	public:
		ImGuiRenderer();
		~ImGuiRenderer();

		void Begin();
		void End();


	private:
		void BeginDockSpace();
		void EndDockSpace();
		RHI::RHIFramebuffer* GetAvailableFramebuffer()const;

		RHI::RHIRenderPass* m_RenderPass;
		std::vector<RHI::RHIFramebuffer*>m_Framebuffers;

		uint8_t m_AvailableFramebufferIndex;
		RHI::ESampleMode m_SameMode;
		bool m_EnableDepth;

	};
}