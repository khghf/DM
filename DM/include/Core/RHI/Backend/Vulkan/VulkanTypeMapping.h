#pragma once
#include"Core/RHI/RHITypes.h"
#include<vulkan/vulkan.h>

namespace DM::RHI
{
	inline VkFormat ToVkFormat(ERHIFormat format)
	{
		switch (format)
		{
			// ============ 纹理/像素格式 ============
		case ERHIFormat::R8_UNorm:           return VK_FORMAT_R8_UNORM;
		case ERHIFormat::R8G8_UNorm:         return VK_FORMAT_R8G8_UNORM;
		case ERHIFormat::R8G8B8_UNorm:       return VK_FORMAT_R8G8B8_UNORM;
		case ERHIFormat::R8G8B8A8_UNorm:     return VK_FORMAT_R8G8B8A8_UNORM;
		case ERHIFormat::B8G8R8A8_UNorm:     return VK_FORMAT_B8G8R8A8_UNORM;
		case ERHIFormat::R32_Float:          return VK_FORMAT_R32_SFLOAT;
		case ERHIFormat::R32G32_Float:       return VK_FORMAT_R32G32_SFLOAT;
		case ERHIFormat::R32G32B32_Float:    return VK_FORMAT_R32G32B32_SFLOAT;
		case ERHIFormat::R32G32B32A32_Float: return VK_FORMAT_R32G32B32A32_SFLOAT;
		case ERHIFormat::D24_UNorm_S8_UInt:  return VK_FORMAT_D24_UNORM_S8_UINT;
		case ERHIFormat::D32_Float:          return VK_FORMAT_D32_SFLOAT;
		case ERHIFormat::R32_Int:            return VK_FORMAT_R32_SINT;

			// ============ 顶点属性格式 - 浮点 ============
		case ERHIFormat::Float:              return VK_FORMAT_R32_SFLOAT;
		case ERHIFormat::Float2:             return VK_FORMAT_R32G32_SFLOAT;
		case ERHIFormat::Float3:             return VK_FORMAT_R32G32B32_SFLOAT;
		case ERHIFormat::Float4:             return VK_FORMAT_R32G32B32A32_SFLOAT;

			// ============ 顶点属性格式 - 有符号整数 ============
		case ERHIFormat::Int:                return VK_FORMAT_R32_SINT;
		case ERHIFormat::Int2:               return VK_FORMAT_R32G32_SINT;
		case ERHIFormat::Int3:               return VK_FORMAT_R32G32B32_SINT;
		case ERHIFormat::Int4:               return VK_FORMAT_R32G32B32A32_SINT;

			// ============ 顶点属性格式 - 无符号整数 ============
		case ERHIFormat::Uint:               return VK_FORMAT_R32_UINT;
		case ERHIFormat::Uint2:              return VK_FORMAT_R32G32_UINT;
		case ERHIFormat::Uint3:              return VK_FORMAT_R32G32B32_UINT;
		case ERHIFormat::Uint4:              return VK_FORMAT_R32G32B32A32_UINT;

			// ============ 顶点属性格式 - 矩阵 ============
			// 注意：Vulkan 不直接支持矩阵格式，需要展开为向量
			// 但为了完整性，这里映射到对应的向量格式
		case ERHIFormat::Mat3:               return VK_FORMAT_R32G32B32A32_SFLOAT; // 3x3 展开为 vec4
		case ERHIFormat::Mat4:               return VK_FORMAT_R32G32B32A32_SFLOAT; // 4x4 展开为 vec4

		case ERHIFormat::Unknown:
		default:                             return VK_FORMAT_R8G8B8A8_UNORM;
		}
	}

	inline ERHIFormat ToRHIFormat(VkFormat format)
	{
		switch (format)
		{
			// ============ 纹理/像素格式 ============
		case VK_FORMAT_R8_UNORM:             return ERHIFormat::R8_UNorm;
		case VK_FORMAT_R8G8_UNORM:           return ERHIFormat::R8G8_UNorm;
		case VK_FORMAT_R8G8B8_UNORM:         return ERHIFormat::R8G8B8_UNorm;
		case VK_FORMAT_R8G8B8A8_UNORM:       return ERHIFormat::R8G8B8A8_UNorm;
		case VK_FORMAT_B8G8R8A8_UNORM:       return ERHIFormat::B8G8R8A8_UNorm;
		case VK_FORMAT_R32_SFLOAT:           return ERHIFormat::R32_Float;
		case VK_FORMAT_R32G32_SFLOAT:        return ERHIFormat::R32G32_Float;
		case VK_FORMAT_R32G32B32_SFLOAT:     return ERHIFormat::R32G32B32_Float;
		case VK_FORMAT_R32G32B32A32_SFLOAT:  return ERHIFormat::R32G32B32A32_Float;
		case VK_FORMAT_D24_UNORM_S8_UINT:    return ERHIFormat::D24_UNorm_S8_UInt;
		case VK_FORMAT_D32_SFLOAT:           return ERHIFormat::D32_Float;
		case VK_FORMAT_R32_SINT:             return ERHIFormat::R32_Int;

			// ============ 顶点属性格式 - 浮点 ============
		case VK_FORMAT_R32G32_SINT:          return ERHIFormat::Int2;
		case VK_FORMAT_R32G32B32_SINT:       return ERHIFormat::Int3;
		case VK_FORMAT_R32G32B32A32_SINT:    return ERHIFormat::Int4;

			// ============ 顶点属性格式 - 无符号整数 ============
		case VK_FORMAT_R32_UINT:             return ERHIFormat::Uint;
		case VK_FORMAT_R32G32_UINT:          return ERHIFormat::Uint2;
		case VK_FORMAT_R32G32B32_UINT:       return ERHIFormat::Uint3;
		case VK_FORMAT_R32G32B32A32_UINT:    return ERHIFormat::Uint4;

			// ============ 通用 sRGB 格式 ============
		case VK_FORMAT_R8G8B8A8_SRGB:        return ERHIFormat::R8G8B8A8_UNorm;  // 近似
		case VK_FORMAT_B8G8R8A8_SRGB:        return ERHIFormat::B8G8R8A8_UNorm;  // 近似

		case VK_FORMAT_UNDEFINED:
		default:                             return ERHIFormat::Unknown;
		}
	}
	inline uint32_t FormatByteSize(ERHIFormat format)
	{
		return GetRHIFormatByteSize(format);
	}

	
	inline VkPrimitiveTopology ToVkTopology(EPrimitiveTopology topo)
	{
		switch (topo)
		{
		case EPrimitiveTopology::TriangleList: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		case EPrimitiveTopology::TriangleStrip:return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		case EPrimitiveTopology::LineList:     return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case EPrimitiveTopology::LineStrip:    return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		case EPrimitiveTopology::PointList:    return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		default:                               return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}
	}

	
	inline VkPolygonMode ToVkPolygonMode(EPolygonMode mode)
	{
		switch (mode)
		{
		case EPolygonMode::Fill: return VK_POLYGON_MODE_FILL;
		case EPolygonMode::Line: return VK_POLYGON_MODE_LINE;
		case EPolygonMode::Point:return VK_POLYGON_MODE_POINT;
		default:                 return VK_POLYGON_MODE_FILL;
		}
	}

	
	inline VkCullModeFlags ToVkCullMode(ECullMode mode)
	{
		switch (mode)
		{
		case ECullMode::None:  return VK_CULL_MODE_NONE;
		case ECullMode::Back:  return VK_CULL_MODE_BACK_BIT;
		case ECullMode::Front: return VK_CULL_MODE_FRONT_BIT;
		case ECullMode::All:   return VK_CULL_MODE_FRONT_AND_BACK;
		default:               return VK_CULL_MODE_NONE;
		}
	}

	inline VkSampleCountFlagBits ToVkSampleCount(ESampleMode mode)
	{
		switch (mode)
		{
		case DM::RHI::ESampleMode::x1:
			return VK_SAMPLE_COUNT_1_BIT;
			break;
		case DM::RHI::ESampleMode::x2:
			return VK_SAMPLE_COUNT_2_BIT;
			break;
		case DM::RHI::ESampleMode::x4:
			return VK_SAMPLE_COUNT_4_BIT;
			break;
		case DM::RHI::ESampleMode::x8:
			return VK_SAMPLE_COUNT_8_BIT;
			break;
		case DM::RHI::ESampleMode::x16:
			return VK_SAMPLE_COUNT_16_BIT;
			break;
		case DM::RHI::ESampleMode::x32:
			return VK_SAMPLE_COUNT_32_BIT;
			break;
		case DM::RHI::ESampleMode::x64:
			return VK_SAMPLE_COUNT_64_BIT;
			break;
		default:
			return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
			break;
		}
	}

	inline VkShaderStageFlagBits ToVkShaderStage(EShaderStage stage)
	{
		switch (stage)
		{
		case EShaderStage::Vertex:   return VK_SHADER_STAGE_VERTEX_BIT;
		case EShaderStage::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
		case EShaderStage::Geometry: return VK_SHADER_STAGE_GEOMETRY_BIT;
		case EShaderStage::Compute:  return VK_SHADER_STAGE_COMPUTE_BIT;
		default:                     return VK_SHADER_STAGE_VERTEX_BIT;
		}
	}

	inline VkDescriptorType ToVkDescriptorType(EDescriptorType type)
	{
		switch (type)
		{
		case EDescriptorType::UniformBuffer:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case EDescriptorType::DynamicUniformBuffer:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;  
		case EDescriptorType::StorageBuffer:
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		case EDescriptorType::DynamicStorageBuffer:
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;  
		case EDescriptorType::CombinedImageSampler:
			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		case EDescriptorType::SampledImage:
			return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		case EDescriptorType::Sampler:
			return VK_DESCRIPTOR_TYPE_SAMPLER;
		case EDescriptorType::StorageImage:
			return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		case EDescriptorType::InputAttachment:
			return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		case EDescriptorType::Unknown:
		default:
			return VK_DESCRIPTOR_TYPE_MAX_ENUM;
		}
	}

	inline VkDescriptorType ToVkDescriptorType(EResourceType type)
	{
		switch (type)
		{
		case EResourceType::UniformBuffer:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case EResourceType::Texture:
			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		case EResourceType::VertexBuffer:
		case EResourceType::Indexbuffer:
		case EResourceType::Shader:
		case EResourceType::Pipeline:
		case EResourceType::CommandList:
		case EResourceType::Swapchain:
		case EResourceType::Unknown:
		default:
			return VK_DESCRIPTOR_TYPE_MAX_ENUM;
		}
	}

	inline VkBlendFactor ToVkBlendFactor(EBlendFactor factor)
	{
		switch (factor)
		{
		case EBlendFactor::Zero:               return VK_BLEND_FACTOR_ZERO;
		case EBlendFactor::One:                return VK_BLEND_FACTOR_ONE;
		case EBlendFactor::SrcAlpha:           return VK_BLEND_FACTOR_SRC_ALPHA;
		case EBlendFactor::OneMinusSrcAlpha:   return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		case EBlendFactor::DstAlpha:           return VK_BLEND_FACTOR_DST_ALPHA;
		case EBlendFactor::OneMinusDstAlpha:   return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		case EBlendFactor::SrcColor:           return VK_BLEND_FACTOR_SRC_COLOR;
		case EBlendFactor::OneMinusSrcColor:   return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		case EBlendFactor::DstColor:           return VK_BLEND_FACTOR_DST_COLOR;
		case EBlendFactor::OneMinusDstColor:   return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
		default:                               return VK_BLEND_FACTOR_ZERO;
		}
	}

	inline VkBlendOp ToVkBlendOp(EBlendOp op)
	{
		switch (op)
		{
		case EBlendOp::Add:            return VK_BLEND_OP_ADD;
		case EBlendOp::Subtract:       return VK_BLEND_OP_SUBTRACT;
		case EBlendOp::ReverseSubtract:return VK_BLEND_OP_REVERSE_SUBTRACT;
		case EBlendOp::Min:            return VK_BLEND_OP_MIN;
		case EBlendOp::Max:            return VK_BLEND_OP_MAX;
		default:                       return VK_BLEND_OP_ADD;
		}
	}

	inline VkCompareOp ToVkCompareOp(ECompareFunc func)
	{
		switch (func)
		{
		case ECompareFunc::Never:    return VK_COMPARE_OP_NEVER;
		case ECompareFunc::Less:     return VK_COMPARE_OP_LESS;
		case ECompareFunc::Equal:    return VK_COMPARE_OP_EQUAL;
		case ECompareFunc::LEqual:   return VK_COMPARE_OP_LESS_OR_EQUAL;
		case ECompareFunc::Greater:  return VK_COMPARE_OP_GREATER;
		case ECompareFunc::NotEqual: return VK_COMPARE_OP_NOT_EQUAL;
		case ECompareFunc::GEqual:   return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case ECompareFunc::Always:   return VK_COMPARE_OP_ALWAYS;
		default:                     return VK_COMPARE_OP_ALWAYS;
		}
	}

	inline VkImageType ToVkImageType(ERHITextureType type)
	{
		switch (type)
		{
		case ERHITextureType::Texture1D:
		case ERHITextureType::Texture1DArray:
			return VK_IMAGE_TYPE_1D;

		case ERHITextureType::Texture2D:
		case ERHITextureType::Texture2DArray:
		case ERHITextureType::Texture2DMS:
		case ERHITextureType::Texture2DMSArray:
		case ERHITextureType::TextureCube:
		case ERHITextureType::TextureCubeArray:
		case ERHITextureType::RenderTarget:
		case ERHITextureType::DepthStencil:
			return VK_IMAGE_TYPE_2D;

		case ERHITextureType::Texture3D:
			return VK_IMAGE_TYPE_3D;

		case ERHITextureType::TextureBuffer:
		case ERHITextureType::Unknown:
		default:
			return VK_IMAGE_TYPE_2D;
		}
	}

	inline VkImageViewType ToVkImageViewType(ERHITextureType type)
	{
		switch (type)
		{
		case ERHITextureType::Texture1D:        return VK_IMAGE_VIEW_TYPE_1D;
		case ERHITextureType::Texture1DArray:   return VK_IMAGE_VIEW_TYPE_1D_ARRAY;

		case ERHITextureType::Texture2D:
		case ERHITextureType::Texture2DMS:
		case ERHITextureType::RenderTarget:
		case ERHITextureType::DepthStencil:
			return VK_IMAGE_VIEW_TYPE_2D;

		case ERHITextureType::Texture2DArray:
		case ERHITextureType::Texture2DMSArray:
			return VK_IMAGE_VIEW_TYPE_2D_ARRAY;

		case ERHITextureType::Texture3D:        return VK_IMAGE_VIEW_TYPE_3D;
		case ERHITextureType::TextureCube:      return VK_IMAGE_VIEW_TYPE_CUBE;
		case ERHITextureType::TextureCubeArray: return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;

		case ERHITextureType::TextureBuffer:
		case ERHITextureType::Unknown:
		default:
			return VK_IMAGE_VIEW_TYPE_2D;
		}
	}
} // namespace DM::RHI