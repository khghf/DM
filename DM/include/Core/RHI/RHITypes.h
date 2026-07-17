#pragma once
#include<Core/Core.h>
#include<cstdint>

namespace DM::RHI
{
	// ============================================================
	// 数据格式
	// ============================================================
	enum class EPixelFormat : uint8_t
	{
		Unknown,

		// 颜色格式
		R8_UNorm,
		R8G8_UNorm,
		R8G8B8_UNorm,
		R8G8B8A8_UNorm,
		R32_Float,
		R32G32_Float,
		R32G32B32_Float,
		R32G32B32A32_Float,

		// 深度/模板
		D24_UNorm_S8_UInt,
		D32_Float,

		// 整数附件
		R32_Int,
	};

	inline uint32_t GetPixelFormatByteSize(EPixelFormat fmt)
	{
		switch (fmt)
		{
		case EPixelFormat::R8_UNorm:            return 1;
		case EPixelFormat::R8G8_UNorm:          return 2;
		case EPixelFormat::R8G8B8_UNorm:        return 3;
		case EPixelFormat::R8G8B8A8_UNorm:      return 4;
		case EPixelFormat::R32_Float:           return 4;
		case EPixelFormat::R32G32_Float:        return 8;
		case EPixelFormat::R32G32B32_Float:     return 12;
		case EPixelFormat::R32G32B32A32_Float:  return 16;
		case EPixelFormat::D24_UNorm_S8_UInt:   return 4;
		case EPixelFormat::D32_Float:           return 4;
		case EPixelFormat::R32_Int:             return 4;
		default:                                return 0;
		}
	}

	// ============================================================
	// 图元拓扑
	// ============================================================
	enum class EPrimitiveTopology : uint8_t
	{
		TriangleList,
		TriangleStrip,
		LineList,
		LineStrip,
		PointList,
	};

	// ============================================================
	// 混合状态
	// ============================================================
	enum class EBlendFactor : uint8_t
	{
		Zero, One,
		SrcAlpha, OneMinusSrcAlpha,
		DstAlpha, OneMinusDstAlpha,
		SrcColor, OneMinusSrcColor,
		DstColor, OneMinusDstColor,
	};

	enum class EBlendOp : uint8_t
	{
		Add, Subtract, ReverseSubtract, Min, Max,
	};

	// ============================================================
	// 深度/模板状态
	// ============================================================
	enum class ECompareFunc : uint8_t
	{
		Never, Less, Equal, LEqual, Greater, NotEqual, GEqual, Always,
	};

	// ============================================================
	// 描述符结构
	// ============================================================
	struct RHIBufferDesc
	{
		uint32_t SizeBytes = 0;
		bool bDynamic = false;  // 频繁更新 / CPU 写入
	};

	struct RHITextureDesc
	{
		uint32_t Width = 1;
		uint32_t Height = 1;
		EPixelFormat Format = EPixelFormat::R8G8B8A8_UNorm;
	};

	struct RHIFrameBufferDesc
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t Samples = 1;
		struct AttachmentDesc
		{
			EPixelFormat Format = EPixelFormat::Unknown;
		};
		std::vector<AttachmentDesc> Attachments;
	};

	// ============================================================
	// 驱动类型枚举（对齐现有 RendererAPI::EAPI）
	// ============================================================
	enum class EDriver : uint8_t
	{
		None,
		OpenGL,
	};

} // namespace DM::RHI
