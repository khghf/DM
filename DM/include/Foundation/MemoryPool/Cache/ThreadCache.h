#pragma once
#include<array>
#include"CentralCache.h"
namespace DM
{

	class ThreadCache
	{
	public:
		static ThreadCache* Inst()
		{
			static thread_local ThreadCache instance;
			return &instance;
		}
		void*	Allocate(size_t size);
		void	DeAllocate(void* ptr, size_t size);
		void Reset()
		{
			_FreeList.fill(nullptr);
			_FreeListSize.fill(0);
		}
	private:
		ThreadCache()
		{
			Reset();
		}
		/// <summary>
		/// 从中央缓存中获取指定索引的内存块。
		/// </summary>
		/// <param name="index">要从中央缓存中获取的内存块的索引。</param>
		/// <returns>指向从中央缓存中获取的内存块的指针。如果获取失败，可能返回空指针。</returns>
		void*	FetchFromCentralCache(size_t index);
		/// <summary>
		/// 将内存块还给中央缓存。
		/// </summary>
		/// <param name="start">指向要返回的内存块的起始地址。</param>
		/// <param name="size">内存块的大小（以字节为单位）。</param>
		void	ReturnToCentralCache(void* start, size_t size);	
		/// <summary>
		/// 获取批处理的数量。
		/// </summary>
		/// <param name="size">输入的大小，用于计算批处理数量。</param>
		/// <returns>批处理的数量，表示为一个无符号整数。</returns>
		size_t	GetBatchNum(size_t size);						

		/// <summary>
		/// 判断是否应返回到中央缓存。
		/// </summary>
		/// <param name="index">要检查的索引值。</param>
		/// <returns>如果应返回到中央缓存，则返回 true；否则返回 false。</returns>
		bool	ShouldReturnToCentralCache(size_t index);	
	private:
		// 每个线程的自由链表数组    
		std::array<void*, FREE_LIST_SIZE> _FreeList;
		//存储每个链表所剩的空闲结点数量
		std::array<size_t, FREE_LIST_SIZE> _FreeListSize; 
	};
}


