#pragma once
#include "Renderer.h"
#include <Foundation/MMM/Reference.h>
#include <Foundation/Math/Vector.h>
#include <Foundation/Math/Matrix.h>

namespace DM
{
	class Texture2D;
}

namespace DM
{
	struct alignas(16) GlobalData
	{
		Matrix4 View{ 1.f };
		Matrix4 Proj{ 1.f };
	};

	struct alignas(4)InstanceData
	{
		Matrix4	Model{ 1.f };
		Vector4	Color{ 1.f,1.f,1.f,1.f };
	};

	struct InstanceBatch
	{
		RHI::RHIBuffer*				VertexBuffer;
		RHI::RHIBuffer*				IndexBuffer;
		RHI::RHIBuffer*				InstanceBuffer;

		RHI::RHIDescriptorSet*		DescriptorSet;
		uint32_t					IndexCount;
		std::vector<InstanceData>	Instances;

		~InstanceBatch()
		{
			delete VertexBuffer;
			delete IndexBuffer;
			delete InstanceBuffer;
			delete DescriptorSet;
		}
	};

	class TriangleRenderer : public Renderer
	{
		struct RenderData
		{
			constexpr static uint32_t m_MaxVertexCount = 10000;
			constexpr static uint32_t m_MaxIndexCount = 10000 * 4;

			std::array<float, m_MaxVertexCount> m_VertexData;
			std::array<uint32_t, m_MaxIndexCount> m_IndexData;

			void Clear()
			{
				m_VertexPtr = 0;
				m_IndexPtr = 0;
			}

			uint32_t AddVertex(const Vector3& pos, const Vector3& color, const Vector2& uv)
			{
				uint32_t baseIndex = m_VertexPtr / 8; 

				m_VertexData[m_VertexPtr++] = pos.x;
				m_VertexData[m_VertexPtr++] = pos.y;
				m_VertexData[m_VertexPtr++] = pos.z;
				m_VertexData[m_VertexPtr++] = color.x;
				m_VertexData[m_VertexPtr++] = color.y;
				m_VertexData[m_VertexPtr++] = color.z;
				m_VertexData[m_VertexPtr++] = uv.x;
				m_VertexData[m_VertexPtr++] = uv.y;

				return baseIndex;
			}

			void AddIndex(uint32_t index)
			{
				m_IndexData[m_IndexPtr++] = index;
			}

			uint32_t GetVertexPtr() const { return m_VertexPtr; }
			uint32_t GetIndexPtr() const { return m_IndexPtr; }

		private:
			uint32_t m_VertexPtr = 0;  // 以 float 为单位
			uint32_t m_IndexPtr = 0;   // 以 uint32_t 为单位
		};
		void UpdateGlobalData(int index);
		void UpdateInstanceData(int index);
		bool EnsurePipelineResources();
	public:

		TriangleRenderer();
		~TriangleRenderer();

		virtual void BeginFrame() override;
		virtual void Update(float deltaTime) override;
		virtual void EndFrame() override;
	
		virtual void Flush() override;
		virtual void FlushForce() override;

		virtual RHI::RHIFramebuffer* GetFramebuffer() const override { return GetAvailableFramebuffer(); }
		virtual RHI::RHISwapchain* GetSwapchain() const override { return m_Swapchain; }
		virtual RHI::RHIRenderPass* GetRenderPass() const override { return m_RenderPass; }
		virtual RHI::RHIPipeline* GetPipeline() const override { return m_Pipeline; }
		virtual RHI::RHICommandList* GetCommandList() const override { return m_Cmd; }

	private:
		RHI::RHIFramebuffer* GetAvailableFramebuffer() const;
		void UpdateRenderData();

		InstanceBatch* CreateInstanceBatch();

		void DrawQuad(const Vector3& pos, float size) {}
		void DrawQuad(const Matrix4& transform, const Vector4& color);
	private:

		RHI::RHICommandList* m_Cmd;

		RHI::RHIRenderPass*		m_RenderPass;
		RHI::RHISwapchain*		m_Swapchain;
		RHI::RHIPipeline*		m_Pipeline;
		//SPtr<Texture2D>			m_Texture;
		RHI::RHIShaderProgram*	m_ShaderProgram;
		
		constexpr static uint32_t MAX_INSTANCE_BYTES = 1024 * 1024 * 16;
		constexpr static uint32_t MAX_INSTANCE_COUNT = MAX_INSTANCE_BYTES/sizeof(InstanceData);

		std::unordered_map<uint32_t, std::vector<InstanceBatch*>>m_InstanceBatchs;

		std::vector<RHI::RHIBuffer*>				m_GlobalBuffers;
		std::vector<RHI::RHIDescriptorSet*>			m_GlobalSets;
		std::vector<RHI::RHIFramebuffer*>			m_Framebuffers;


		uint8_t				m_AvailableFramebufferIndex = 0;
		RHI::ESampleMode	m_SameMode;
		bool				m_EnableDepth;
		bool				m_bPipelineReady = false;	// 内置 shader 管线资源是否已就绪(惰性创建)
		bool				m_bFrameActive = true;		// 本帧是否可渲染(管线未就绪时跳过)
		RenderData			m_RenderData{};
	};
}