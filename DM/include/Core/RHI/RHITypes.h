#pragma once
#include<cstdint>
#include<vector>
#include<Foundation/MemoryPool/MemoryPool.h>

#include<Core/Log.h>
namespace DM::RHI
{
	class RHIShader;
	class RHIFramebuffer;
	class RHITexture;
	class RHISwapchain;
	class RHIResource;
	class RHIRenderPass;
	class RHIShaderProgram;
	class RHIBuffer;
	enum class EAPIType : uint8_t
	{
		None,    
		OpenGL, 
		Vulkan,
	};
	

	enum class EResourceType : uint8_t
	{
		Unknown,
		Buffer,
		VertexBuffer,
		Indexbuffer,
		UniformBuffer,
		Texture,
		Shader,
		ShaderProgram,
		Pipeline,
		CommandList,
		Swapchain,
		RenderPass,
		Framebuffer,
	};

	/// <summary>
	/// 合并了像素格式和顶点属性格式，提供统一的格式描述。
	/// </summary>
	enum class EFormat : uint8_t
	{
		Unknown,  

		R8_UNorm,           // 单通道灰度	1字节	0-255映射到0.0-1.0
		R8G8_UNorm,         // 双通道RG		2字节	0-255映射到0.0-1.0
		R8G8B8_UNorm,       // 三通道RGB	3字节	0-255映射到0.0-1.0
		R8G8B8A8_UNorm,     // 四通道RGBA	4字节	0-255映射到0.0-1.0
		B8G8R8A8_UNorm,     // 四通道RGBA	4字节 	0-255映射到0.0-1.0

		R32_Float,          // 单通道32位浮点
		R32G32_Float,       // 双通道32位浮点(vec2)
		R32G32B32_Float,    // 三通道32位浮点(vec3)
		R32G32B32A32_Float, // 四通道32位浮点(vec4)

		// 深度/模板格式
		D24_UNorm_S8_UInt,  // 24位深度(归一化)+8位模板(整数)
		D32_Float,          // 32位浮点深度(无模板)

		R32_Int,            // 单通道32位有符号整数

		// 浮点标量和向量
		Float,              // float 
		Float2,             // vec2 
		Float3,             // vec3 
		Float4,             // vec4 

		// 有符号整数
		Int,                // int 
		Int2,               // ivec2 
		Int3,               // ivec3 
		Int4,               // ivec4 

		// 无符号整数
		Uint,               // uint 
		Uint2,              // uvec2 
		Uint3,              // uvec3 
		Uint4,              // uvec4 

		// 矩阵
		Mat3,               // 3×3 
		Mat4,               // 4×4 
	};

	
	inline uint32_t GetRHIFormatByteSize(EFormat format)
	{
		switch (format)
		{
			// 1 字节
		case EFormat::R8_UNorm:
			return 1;

			// 2 字节
		case EFormat::R8G8_UNorm:
			return 2;

			// 3 字节
		case EFormat::R8G8B8_UNorm:
			return 3;

			// 4 字节
		case EFormat::R8G8B8A8_UNorm:
		case EFormat::B8G8R8A8_UNorm:
		case EFormat::R32_Float:
		case EFormat::R32_Int:
		case EFormat::D24_UNorm_S8_UInt:
		case EFormat::D32_Float:
		case EFormat::Float:
		case EFormat::Int:
		case EFormat::Uint:
			return 4;

			// 8 字节
		case EFormat::R32G32_Float:
		case EFormat::Float2:
		case EFormat::Int2:
		case EFormat::Uint2:
			return 8;

			// 12 字节
		case EFormat::R32G32B32_Float:
		case EFormat::Float3:
		case EFormat::Int3:
		case EFormat::Uint3:
			return 12;

			// 16 字节
		case EFormat::R32G32B32A32_Float:
		case EFormat::Float4:
		case EFormat::Int4:
		case EFormat::Uint4:
			return 16;

			// 36 字节
		case EFormat::Mat3:
			return 36;

			// 64 字节
		case EFormat::Mat4:
			return 64;

		default:
			return 0;
		}
	}

	
	inline bool IsPixelFormat(EFormat format)
	{
		return format >= EFormat::R8_UNorm && format <= EFormat::R32_Int;
	}

	
	inline bool IsVertexFormat(EFormat format)
	{
		return format >= EFormat::Float && format <= EFormat::Mat4;
	}


	
	enum class EPrimitiveTopology : uint8_t
	{
		TriangleList,  // 每3个顶点组成一个独立三角形。最常用、最直观。(demo 用这个)
		TriangleStrip, // 三角形带：v0v1v2 一个三角，v1v2v3 又一个……相邻三角共享边。
		LineList,      // 每2个顶点一条独立线段。
		LineStrip,     // 线带：相邻线段共享端点。
		PointList,     // 每个顶点画成一个点。
	};

	enum class EBlendFactor : uint8_t
	{
		Zero,            // 乘 0(即不使用该项)
		One,             // 乘 1(直接用)
		SrcAlpha,        // 用源颜色的 alpha 作为系数
		OneMinusSrcAlpha,// 用 (1 - 源 alpha) 作为系数 —— 经典"半透明"：Src*alpha + Dst*(1-alpha)
		DstAlpha,        // 用目标 alpha
		OneMinusDstAlpha,// 用 (1 - 目标 alpha)
		SrcColor,        // 用源颜色本身
		OneMinusSrcColor,// 用 (1 - 源颜色)
		DstColor,        // 用目标颜色
		OneMinusDstColor,// 用 (1 - 目标颜色)
	};

	enum class EBlendOp : uint8_t
	{
		Add,           // 相加(最常用，透明混合就用 Src + Dst)
		Subtract,      // 相减：Src - Dst
		ReverseSubtract,// 反向相减：Dst - Src
		Min,           // 取较小值
		Max,           // 取较大值
	};

	enum class ECompareFunc : uint8_t
	{
		Never,        // 永远不通过(该像素被丢弃)
		Less,         // 新值 < 旧值才通过(标准"近处优先")
		Equal,        // 相等才通过
		LEqual,       // 小于等于才通过
		Greater,      // 大于才通过
		NotEqual,     // 不等才通过
		GEqual,       // 大于等于才通过
		Always,       // 永远通过(不做深度剔除，所有像素都画)
	};

	enum class EPolygonMode : uint8_t { Fill, Line, Point };
	enum class ECullMode : uint8_t { None, Front, Back, All };
	enum class ESampleMode:uint8_t{x1,x2,x4,x8,x16,x32,x64};

	enum class EShaderStage : uint8_t
	{
		Unknown,
		Vertex,    
		Fragment, 
		Geometry, 
		Compute,  
		TessellationControl,	
		TessellationEvaluation,
	};
	enum class ETextureType : uint8_t
	{
		Unknown,        // 未知/未指定类型

		// ============ 2D 纹理 ============
		Texture2D,      // 标准 2D 纹理(单采样，最常用)
		Texture2DArray, // 2D 纹理数组(多个 2D 纹理层)
		Texture2DMS,    // 2D 多重采样纹理(MSAA)
		Texture2DMSArray,// 2D 多重采样纹理数组

		// ============ 3D 纹理 ============
		Texture3D,      // 3D 体积纹理(体素数据，如雾效、体积光)

		// ============ 1D 纹理 ============
		Texture1D,      // 1D 纹理(较少用，可用于查找表)
		Texture1DArray, // 1D 纹理数组

		// ============ 立方体贴图 ============
		TextureCube,    // 立方体贴图(6个面，用于天空盒、环境映射)
		TextureCubeArray,// 立方体贴图数组

		// ============ 缓冲纹理 ============
		TextureBuffer,  // 缓冲纹理(将缓冲区作为纹理采样，用于顶点纹理等)

		// ============ 渲染目标专用 ============
		RenderTarget,   // 渲染目标纹理(可写入，通常与 FrameBuffer 配合)
		DepthStencil,   // 深度模板纹理(专门用于深度/模板测试)
	};

	enum class EBufferType : uint8_t
	{
		VertexBuffer,            // 顶点缓冲
		IndexBuffer,             // 索引缓冲
		UniformBuffer,           // UBO
		DynamicUniformBuffer,    // 动态 UBO
		StorageBuffer,           // SSBO
		DynamicStorageBuffer,    // 动态 SSBO
		IndirectBuffer,          // 间接绘制参数
		StagingBuffer,           // 暂存缓冲
	};
	
	/*
	/// <summary>
	/// 表示单一顶点属性
	/// </summary>
	struct VertexProperty
	{
		template<typename T>
		VertexProperty(const std::vector<T>&data)
		{
			m_SizeBytes = sizeof(T) * data.size();
			Data = MemoryPool::Allocate(m_SizeBytes);
			if (!Data)
			{
				throw std::bad_alloc();
				return;
			}
			memcpy(Data, data.data(), m_SizeBytes);
		}

		template<typename T>
		VertexProperty(std::initializer_list<T> data)
		{
			m_SizeBytes = sizeof(T) * data.size();
			Data = MemoryPool::Allocate(m_SizeBytes);
			if (!Data)
			{
				throw std::bad_alloc();
				return;
			}
			memcpy(Data, data.begin(), m_SizeBytes);
		}


		~VertexProperty()
		{
			if(Data)MemoryPool::DeAllocate(Data, m_SizeBytes);
			
		}

		VertexProperty(VertexProperty&& other)noexcept
		{
			this->Data = other.Data;
			this->m_SizeBytes = other.m_SizeBytes;
			other.Data = nullptr;

		}
		VertexProperty(const VertexProperty& other)
		{
			this->Data = other.Data;
			this->m_SizeBytes = other.m_SizeBytes;
			const_cast<VertexProperty*>(&other)->Data = nullptr;
		}

		void* Data = nullptr;
		uint32_t m_SizeBytes = 0;
	};
	
	/// <summary>
	/// 表示一个顶点(顶点是由多个顶点属性组合而成的)
	/// </summary>
	struct Vertex
	{
		Vertex(const std::vector<VertexProperty>& properties)
		{
			for (const auto& in : properties)
			{
				m_SizeBytes += in.m_SizeBytes;
			}
			Data = MemoryPool::Allocate(m_SizeBytes);
			if (!Data)
			{
				throw std::bad_alloc();
				return;
			}
			uint8_t* dest = static_cast<uint8_t*>(Data);
			for (const auto& in : properties)
			{
				memcpy(dest, in.Data, in.m_SizeBytes);
				dest += in.m_SizeBytes;
			}
		}

		Vertex(std::initializer_list<VertexProperty> properties)
		{
			for (const auto& in : properties)
			{
				m_SizeBytes += in.m_SizeBytes;
			}
			Data = MemoryPool::Allocate(m_SizeBytes);
			if (!Data)
			{
				throw std::bad_alloc();
				return;
			}
			uint8_t* dest = static_cast<uint8_t*>(Data);
			for (const auto& in : properties)
			{
				memcpy(dest, in.Data, in.m_SizeBytes);
				dest += in.m_SizeBytes;
			}
		}


		~Vertex()
		{
			if(Data)MemoryPool::DeAllocate(Data, m_SizeBytes);
		}

		Vertex(Vertex&& other)noexcept
		{
			this->Data = other.Data;
			this->m_SizeBytes = other.m_SizeBytes;
			other.Data = nullptr;
		}
		Vertex(const Vertex& other)
		{
			this->Data = other.Data;
			this->m_SizeBytes = other.m_SizeBytes;
			const_cast<Vertex*>(&other)->Data = nullptr;
		}


		void* Data = nullptr;
		uint32_t m_SizeBytes = 0;
	};

	/// <summary>
	/// 表示具有完全一样的属性的顶点的集合
	/// </summary>
	struct VertexSet
	{
		//使用了自定义内存池会触发一次拷贝，并不会直接接管资源
		VertexSet(const std::vector<Vertex>&vertices)
		{
			if (vertices.size() <= 0) return;
			m_SizeBytes = vertices.begin()->m_SizeBytes * vertices.size();
			Data = MemoryPool::Allocate(m_SizeBytes);
			uint8_t* dest = static_cast<uint8_t*>(Data);
			for (const auto& in : vertices)
			{
				memcpy(dest, in.Data, in.m_SizeBytes);
				dest += in.m_SizeBytes;
			}
		}
		VertexSet(std::initializer_list<Vertex> vertices)
		{
			if (vertices.size() <= 0) return;
			m_SizeBytes = vertices.begin()->m_SizeBytes * vertices.size();
			Data = MemoryPool::Allocate(m_SizeBytes);
			uint8_t* dest = static_cast<uint8_t*>(Data);
			for (const auto& in : vertices)
			{
				memcpy(dest, in.Data, in.m_SizeBytes);
				dest += in.m_SizeBytes;
			}
		}
		~VertexSet()
		{
			if(Data)MemoryPool::DeAllocate(Data, m_SizeBytes);
		}

		VertexSet(VertexSet&& other)noexcept
		{
			this->Data = other.Data;
			this->m_SizeBytes = other.m_SizeBytes;
			other.Data = nullptr;

		}
		VertexSet(const VertexSet& other)
		{
			this->Data = other.Data;
			this->m_SizeBytes = other.m_SizeBytes;
			const_cast<VertexSet*>(&other)->Data = nullptr;
		}



		void* Data = nullptr;
		uint32_t m_SizeBytes = 0;
	};
	/// <summary>
	/// 描述顶点的属性布局
	/// </summary>
	struct VertexLayout
	{
		struct VertexLayoutInner
		{
			std::string propertyName;
			EFormat format = EFormat::Unknown;
			uint32_t binding = 0;//数据来源(例：对应着不同的缓冲区)
			uint32_t location = 0;// 但着色器需要按顺序声明
			uint32_t offset; // 无需填写会自动计算
		};
		VertexLayout(std::initializer_list<VertexLayoutInner> layout)
		{
			Layout.reserve(layout.size());
			uint32_t offset = 0;
			uint32_t location = 0;
			for (auto& in : layout)
			{
				auto& inner = Layout.emplace_back(in);
				inner.offset = offset;
				inner.location = location++;
				offset += GetRHIFormatByteSize(inner.format);
			}
			VertexStride = offset;
		}

		std::vector<VertexLayoutInner> Layout;
		uint32_t VertexStride = 0;
	};
	*/
	


	struct RHIBufferDesc
	{
		EBufferType Type;
		uint32_t SizeBytes{};
	};

	struct RHIVertexBufferDesc
	{
		/*RHIVertexBufferDesc(const VertexSet& vertexSet, const VertexLayout& layout)
			: Vertices(vertexSet), Layout(layout)
		{

		}*/
		//VertexSet Vertices;
		//VertexLayout Layout;
		//uint8_t BufferCount = 1;

		uint32_t SizeBytes{};

	};

	struct RHIIndexBufferDesc
	{
		/*RHIIndexBufferDesc(const std::vector<uint32_t>&indices)
		{
			Indices = indices;
		}
		RHIIndexBufferDesc(std::vector<uint32_t>&&indices)
		{
			Indices = std::move(indices);
		}
		std::vector<uint32_t> Indices;
		uint8_t BufferCount = 1;*/

		uint32_t SizeBytes{};
	};

	struct RHIUniformBufferDesc
	{
		EBufferType Type;
		uint32_t	m_SizeBytes;
		bool		m_bDynamic;
	};


	struct RHITextureDesc
	{
		ETextureType Type = ETextureType::Texture2D;  
		uint32_t Width = 0;                                 
		uint32_t Height = 0;                                
		uint32_t Depth = 1;                                 // 深度(仅3D纹理使用)
		uint32_t ArrayLayers = 1;                           // 数组层数(仅数组纹理使用)
		uint32_t MipLevels = 1;                             // Mipmap 级别数
		EFormat Format = EFormat::R8G8B8A8_UNorm;		// 像素格式

		//ERHITextureUsage Usage = ERHITextureUsage::Sampled;// 纹理用途
	};


	enum class ERHIAttachmentUsage : uint8_t 
	{
		Unknown = 0,
		
		ColorTarget,          // 最终画面
		ColorIntermediate,    // 中间渲染目标
		ColorResolve,         // MSAA 解析目标

		DepthTarget,          // 深度缓冲
		
		GBufferAlbedo,        // 延迟渲染：反照率
		GBufferNormal,        // 延迟渲染：法线
		GBufferPosition,      // 延迟渲染：位置
		GBufferMaterial,      // 延迟渲染：材质属性

		ShadowMap,            // 阴影贴图
		SSAOMap,              // 环境光遮蔽
		BloomTarget,          // Bloom中间目标

		
		Transient,            // 临时附件(渲染完丢弃，不写回内存)
		Persistent,           // 持久附件(渲染后仍需读取)
		
		Present,              // 呈现到屏幕
		SwapchainBackBuffer,  // 交换链后缓冲
	};

	struct RHIDeviceDesc
	{
		EAPIType ApiType = EAPIType::Vulkan;
		void* WindowHandle = nullptr;
		bool bEnableValidation = true;
	};

	struct RHISwapchainDesc
	{
		void* WindowHandle = nullptr;
		ESampleMode SampleMode = ESampleMode::x4;
	};

	struct AttachmentDesc
	{
		EFormat Format = EFormat::Unknown;
		ERHIAttachmentUsage Usage = ERHIAttachmentUsage::Unknown;
	};

	struct RHIFramebufferDesc
	{
		uint32_t Width = 0;     
		uint32_t Height = 0;   
		ESampleMode SampleMode= ESampleMode::x4;
		std::vector<AttachmentDesc> Attachments; 
	};

	struct RHIRenderPassDesc
	{
		ESampleMode SampleMode = ESampleMode::x4;
		EFormat	Format;
		bool		EnableDepth = true;
		std::vector<AttachmentDesc> Attachments;
	};

	
	enum class EDescriptorType
	{
		Unknown,
		UniformBuffer,   
		DynamicUniformBuffer,
		StorageBuffer,
		DynamicStorageBuffer,
		CombinedImageSampler,    
		SampledImage,            
		Sampler,                 
		StorageImage,            
		InputAttachment,         
	};

	/// <summary>
	/// 着色器反射信息
	/// </summary>
	struct ShaderReflection
	{
		/// <summary>
		/// 顶点输入属性
		/// </summary>
		struct VertexInput
		{
			uint32_t location;
			EFormat format;
			uint32_t offset;
			uint32_t size;
			std::string name;
		};

		/// <summary>
		/// Uniform成员
		/// </summary>
		struct UniformMember
		{
			std::string name;

			uint32_t offset;

			size_t size;
		};

		/// <summary>
		/// Uniform Buffer
		/// </summary>
		struct UniformBuffer
		{
			/// <summary>
			/// 描述符集索引(Vulkan专属)
			/// </summary>
			uint32_t set;

			uint32_t binding;

			std::string name;

			size_t size;

			uint32_t arraySize;

			EDescriptorType type;

			//bool bStorageBuffer = false;

			//bool bDynamic;

			std::vector<UniformMember> members;
		};

		/// <summary>
		/// 纹理绑定
		/// </summary>
		struct TextureBinding
		{
			/// <summary>
			/// 描述符集索引
			/// </summary>
			uint32_t set;

			uint32_t binding;

			std::string name;

			uint32_t arraySize;
			EDescriptorType type = EDescriptorType::Unknown;
		};

		/// <summary>
		/// 推送常量(Vulkan专属)
		/// </summary>
		struct PushConstant
		{
			EShaderStage stage;

			uint32_t offset;

			size_t size;

			std::vector<UniformMember> members;
		};

		std::vector<VertexInput> vertexInputs;

		std::vector<UniformBuffer> uniformBuffers;

		std::vector<TextureBinding> textures;

		std::vector<PushConstant> pushConstants;
	};

	/// <summary>
	/// 描述 RHI 着色器的结构体。
	/// </summary>
	struct RHIShaderDesc
	{
		EShaderStage Stage = EShaderStage::Vertex;
		const uint32_t* Code = nullptr; // SPIR-V 字节(uint32数组),可用ShaderCompiler从着色器代码文件编译
		size_t CodeBytes = 0;			
		const char* EntryPoint = "main";
		ShaderReflection Reflection;
	};
	struct RHIShaderProgramDesc
	{
		std::vector<RHIShader*>Shaders;
	};


	struct Descriptor
	{
		EDescriptorType m_Type;
		uint32_t		m_Set;
		uint32_t		m_binding;
		std::string		m_Name;
		uint32_t		arraySize;
	};

	struct RHIDescriptorSetDesc
	{
		std::vector<Descriptor>Descriptors;
	};
	/// <summary>
	/// 资源视图用于描述着色器如何引用单一资源
	/// </summary>
	struct RHIResourceView
	{
		uint8_t Set = 0;
		uint32_t Binding;
		EShaderStage ShaderStage;
		RHIResource* Target;

		bool operator<(const RHIResourceView& other) const
		{
			if (Set != other.Set) return Set < other.Set;
			return Binding < other.Binding;
		}

		bool operator==(const RHIResourceView& other) const
		{
			return Binding == other.Binding && Set == other.Set;
		}
	};

	/// <summary>
	/// 资源视图集合用于描述着色器如何引用所需的所有资源
	/// </summary>
	struct RHIResourceViewSet
	{
		RHIResourceViewSet(std::initializer_list<RHIResourceView> viewSet)
		{
			if (viewSet.size() <= 0) return;
			Views.reserve(viewSet.size());
			for (const auto& in : viewSet)
			{
				Views.emplace_back(in);
			}
		}
		std::vector<RHIResourceView> Views;
	};
	

	struct RHIPipelineDesc
	{
		EPrimitiveTopology	Topology =		EPrimitiveTopology::TriangleList;
		ECullMode			CullMode =		ECullMode::Back;
		EPolygonMode		FillMode =		EPolygonMode::Fill;
		ECompareFunc		DepthCompare =	ECompareFunc::Less;
		bool				DepthTest = true;
		bool				DepthWrite = true;
		RHIShaderProgram*	ShaderProgram;
		RHIRenderPass*		RenderPass = nullptr;
	};
	

} // namespace DM::RHI