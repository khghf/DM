#pragma once
#include <cstdint>
#include <Core/Core.h>

namespace DM
{
	// 安全的资源引用句柄。
	// Handle 由 Index + Generation 组成：
	//   - Index 指向资源槽位；
	//   - Generation 用于检测悬空引用——当资源被卸载/复用后
	//     Generation 会改变，此时 Get() 返回 nullptr。
	// 取代裸指针，避免“资源已释放但句柄仍被持有”的悬垂问题。
	struct DM_API ResourceHandle
	{
		using IndexType = uint32_t;
		using GenerationType = uint32_t;

		IndexType Index = 0;
		GenerationType Generation = 0;

		ResourceHandle() = default;
		ResourceHandle(IndexType index, GenerationType generation)
			: Index(index), Generation(generation) {}

		bool operator==(const ResourceHandle& other) const
		{
			return Index == other.Index && Generation == other.Generation;
		}
		bool operator!=(const ResourceHandle& other) const { return !(*this == other); }

		bool IsValid() const { return Index != 0; }
		operator bool() const { return IsValid(); }
	};

	// 句柄的空值常量。
	inline constexpr ResourceHandle NullHandle{ 0, 0 };
}
