#pragma once
#include"Renderer.h"
#include<Foundation/MMM/Reference.h>
namespace DM
{
	class Texture2D;
}
namespace DM
{
	class TriangleRenderer :public Renderer
	{
	public:
		TriangleRenderer();
		~TriangleRenderer();
		
		virtual void BeginFrame() override;
		virtual void UpdateData(float deltaTime)override;
		virtual void EndFrame()override;

		virtual RHI::RHIFramebuffer*	GetFramebuffer()const override { return GetAvailableFramebuffer();}
		virtual RHI::RHISwapchain*		GetSwapchain()const override	{ return m_Swapchain; }
		virtual RHI::RHIRenderPass*		GetRenderPass()const override	{ return m_RenderPass; }
		virtual RHI::RHIPipeline*		GetPipeline()const	override	{ return m_Pipeline; }
		virtual RHI::RHICommandList*	GetCommandList()const override	{ return m_Cmd; }

	private:
		RHI::RHIFramebuffer* GetAvailableFramebuffer()const;
	private:


		RHI::RHICommandList*			m_Cmd;
		RHI::RHIBuffer*					m_Vertex;
		RHI::RHIBuffer*					m_Index;

		std::vector<RHI::RHIBuffer*>	m_Uniform;

		RHI::RHIRenderPass*				m_RenderPass;
		RHI::RHISwapchain*				m_Swapchain;
		RHI::RHIPipeline*				m_Pipeline;
		SPtr<Texture2D>					m_Texture;
		RHI::RHIShaderProgram*			m_ShaderProgram;

		std::vector<RHI::RHIFramebuffer*>m_Framebuffers;
		std::vector<RHI::RHIDescriptorSetGroup*>m_SetGroups;

		uint8_t m_AvailableFramebufferIndex;

		uint32_t m_IndexCount = 0;
		RHI::ESampleMode m_SameMode;
		bool m_EnableDepth;
	};
}