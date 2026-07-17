#include<DMPCH.h>
#include"Core/Render/Renderer/Renderer2D.h"
#include<string_view>
#include<Config.h>
#include<Core/Render/Texture/Texture.h>
#include<Core/Render/UniformBuffer.h>
namespace DM
{
	struct QuadVertex
	{
		Vector3 Pos;	
		Vector2 TexCoord;
		Vector4 Color;
		int TexSlot;
		Vector2 UVTiling;
	};
	
	struct Renderer2DData
	{
		const uint32_t MaxQuadCount = 10000;
		const uint32_t MaxVertexCount = MaxQuadCount*4;
		const uint32_t MaxIndexCount = MaxQuadCount * 6;

		static const uint32_t MaxTextureSlots = 32;
		std::array<SPtr<Texture>, MaxTextureSlots>TextureSlots;
		uint32_t TextureSlotIndex = 1;//0 for white texture

		uint32_t QuadIndexCount = 0;//Quad索引缓冲元素数目
		QuadVertex* QuadVertexBase = nullptr;//Quad顶点缓冲数据基址
		QuadVertex* QuadVertexPtr = nullptr;//Quad顶点缓冲数据边界
		SPtr<VertexBuffer>QuadVertexBuffer;
		SPtr<VertexArray>QuadVertexArray;
		SPtr<Shader>TextureShader;
		SPtr<Texture>WhiteTexture;
		struct CameraData
		{
			Matrix4 ProjectionView = Matrix4(1.f);
		};
		CameraData CameraBuffer;
		SPtr<UniformBuffer> CameraUniformBuffer;
		Renderer2D::Statistics Stats;
	};
	static Renderer2DData* s_Data = nullptr;
	void Renderer2D::Init()
	{
		DM_CORE_ASSERT(s_Data==nullptr, "{}", "Renderer2D repeated initialization");
		s_Data = new Renderer2DData();
		s_Data->QuadVertexBase = new QuadVertex[s_Data->MaxVertexCount];
		uint32_t* quadIndices = new uint32_t[s_Data->MaxIndexCount];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data->MaxIndexCount; i += 6)
		{
			quadIndices[i + 0] = 0 + offset;
			quadIndices[i + 1] = 1 + offset;
			quadIndices[i + 2] = 2 + offset;
			quadIndices[i + 3] = 2 + offset;
			quadIndices[i + 4] = 3 + offset;
			quadIndices[i + 5] = 0 + offset;
			offset += 4;
		}
		SPtr<IndexBuffer>IBO = IndexBuffer::Create(quadIndices, s_Data->MaxIndexCount);
		delete[] quadIndices;
		BufferLayout layout{
				{ShaderDataType::Float3,"a_Pos"},
				{ShaderDataType::Float2,"a_TexCoord"},
				{ShaderDataType::Float4,"a_Color"},
				{ShaderDataType::Int,"a_TexSlot"},
				{ShaderDataType::Float2,"a_UVTiling"},
		};
		s_Data->QuadVertexBuffer = VertexBuffer::Create(s_Data->MaxVertexCount * sizeof(QuadVertex));
		s_Data->QuadVertexBuffer->SetLayout(layout);
		s_Data->QuadVertexArray= VertexArray::Create();
		s_Data->QuadVertexArray->AddVertexBuffer(s_Data->QuadVertexBuffer);
		s_Data->QuadVertexArray->SetIndexBuffer(IBO);
		s_Data->TextureShader= Shader::Create(Config::AssetShader() + "/Texture.glsl");
		s_Data->TextureShader->Bind();
		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		//uint32_t data = 0x00000000;
		uint32_t data = 0xffffffff;
		s_Data->WhiteTexture->SetData(&data, sizeof(uint32_t));
		s_Data->TextureSlots[0] = s_Data->WhiteTexture;
		int32_t textureslot[s_Data->MaxTextureSlots];
		for (int i = 0; i < s_Data->MaxTextureSlots; ++i)textureslot[i] = i;
		s_Data->TextureShader->SetIntArray("u_Textures", textureslot, s_Data->MaxTextureSlots);
		s_Data->CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
	}
	void Renderer2D::ShoutDown()
	{
		DM_CORE_ASSERT(s_Data!=nullptr, "{}", "Renderer2D repeated release");
		delete s_Data;
		s_Data = nullptr;
	}
	void Renderer2D::BeginScene(const SPtr<Camera>& orthoCamera)
	{
		BeginScene(orthoCamera->GetProjectionViewMatrix());
	}

	void Renderer2D::BeginScene(const Matrix4& projectionView)
	{
		s_Data->CameraBuffer.ProjectionView = projectionView;
		s_Data->CameraUniformBuffer->SetData(&s_Data->CameraBuffer, sizeof(Renderer2DData::CameraData));
		s_Data->QuadVertexPtr = s_Data->QuadVertexBase;
		s_Data->QuadIndexCount = 0;
		s_Data->TextureSlotIndex = 1;
		ResetStats();
		DM_CORE_ASSERT(s_Data->QuadVertexPtr == s_Data->QuadVertexBase, "{}", "顶点指针重置失败！");
	}

	void Renderer2D::EndScene()
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data->QuadVertexPtr - (uint8_t*)s_Data->QuadVertexBase);
		if (dataSize == 0)return;
		s_Data->QuadVertexBuffer->SetData(s_Data->QuadVertexBase, dataSize);
	/*	glm::mat4 model(1.f);
		s_Data->TextureShader->SetMat4x4("u_ModelMatrix", model);*/
		s_Data->TextureSlots[0]->Bind(0);
		Flush();
	}
	void Renderer2D::Flush()
	{
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray,s_Data->QuadIndexCount);
		++s_Data->Stats.DrawCall;
	}

	void Renderer2D::DrawQuad(const Vector2& pos, const Vector2& size, const Vector4& color)
	{
		DrawQuad(Vector3(pos,0.f), size, color);
	}
	void Renderer2D::DrawQuad(const Vector3& pos, const Vector2& size, const Vector4& color)
	{
		if (s_Data->QuadIndexCount >= s_Data->MaxIndexCount)FlushAndRest();
		const int texslot = 0;
		const  Vector2 textCoord[4] = {
			{0.f,0.f},
			{0.f,1.f},
			{1.f,1.f},
			{1.f,0.f},
		};
		const Vector3 offset[4] = {
			{ -size.x / 2.f,-size.y / 2.f,pos.z},
			{ -size.x / 2.f,+size.y / 2.f,pos.z},
			{ +size.x / 2.f,+size.y / 2.f,pos.z},
			{ +size.x / 2.f,-size.y / 2.f,pos.z},
		};
		for (int i = 0; i < 4; ++i)
		{
			s_Data->QuadVertexPtr->Pos = pos+offset[i];
			s_Data->QuadVertexPtr->Color = color;
			s_Data->QuadVertexPtr->TexCoord = textCoord[i];
			s_Data->QuadVertexPtr->TexSlot = texslot;
			s_Data->QuadVertexPtr->UVTiling = { 1.f,1.f };
			++s_Data->QuadVertexPtr;
		}
		s_Data->QuadIndexCount += 6;
		++s_Data->Stats.QuadCount;
	}

	void Renderer2D::DrawQuad(const Vector2& pos, const Vector2& size, const SPtr<Texture2D>& texture, const Vector2& uvTiling , const Vector4& color)
	{
		DrawQuad(Vector3(pos,0.f), size, texture, uvTiling, color);
	}

	void Renderer2D::DrawQuad(const Vector3& pos, const Vector2& size, const SPtr<Texture2D>& texture, const Vector2& uvTiling, const Vector4& color)
	{
		if (s_Data->QuadIndexCount >=s_Data->MaxIndexCount)FlushAndRest();
		int texslot = 0;
		for (uint32_t i = 1; i < s_Data->TextureSlotIndex; ++i)
		{
			if (s_Data->TextureSlots[i] && (*s_Data->TextureSlots[i].get() == *texture.get()))
			{
				texslot = i;
				break;
			}
		}
		if (texslot == 0)
		{
			DM_CORE_ASSERT(s_Data->TextureSlotIndex < 32, "{}", "out of  texture slots range(32)");
			texture->Bind(s_Data->TextureSlotIndex);
			texslot =(int)s_Data->TextureSlotIndex;
			s_Data->TextureSlots[texslot] = texture;
			++s_Data->TextureSlotIndex;
		}
		const  Vector2 textCoord[4] = {
			{0.f,0.f},
			{0.f,1.f},
			{1.f,1.f},
			{1.f,0.f},
		};
		const Vector3 offset[4] = {
			{ -size.x / 2.f,-size.y / 2.f,pos.z},
			{ -size.x / 2.f,+size.y / 2.f,pos.z},
			{ +size.x / 2.f,+size.y / 2.f,pos.z},
			{ +size.x / 2.f,-size.y / 2.f,pos.z},
		};
		for (int i = 0; i < 4; ++i)
		{
			s_Data->QuadVertexPtr->Pos = pos + offset[i];
			s_Data->QuadVertexPtr->Color = color;
			s_Data->QuadVertexPtr->TexCoord = textCoord[i];
			s_Data->QuadVertexPtr->TexSlot = texslot;
			s_Data->QuadVertexPtr->UVTiling = uvTiling;
			++s_Data->QuadVertexPtr;
		}
		s_Data->QuadIndexCount += 6;
		++s_Data->Stats.QuadCount;
	}

	void Renderer2D::DrawQuad(const SpriteComponent& sprite, const TransformComponent& transform, const Vector2& uvTiling /*= Vector2(1.f, 1.f)*/)
	{
		Vector3 pos = transform.Location;
		Vector2 size = { 1.f,1.f };
		size.x *= transform.Scale.x;
		size.y *= transform.Scale.y;
		DrawQuad(pos, size, sprite.Color);
	}

	RendererAPI::EAPI Renderer2D::GetAPI() { return RendererAPI::GetAPI(); }
	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data->Stats;
	}
	void Renderer2D::ResetStats()
	{
		s_Data->Stats.DrawCall = 0;
		s_Data->Stats.QuadCount = 0;
	}
	void Renderer2D::FlushAndRest()
	{
		EndScene();
		s_Data->QuadVertexPtr = s_Data->QuadVertexBase;
		s_Data->QuadIndexCount = 0;
	}
}