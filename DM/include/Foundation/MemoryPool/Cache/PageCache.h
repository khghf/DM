#pragma once
#include "PageCache.h"
#include<map>
#include<mutex>
#include<vector>
#ifdef _WIN32
	#include<Windows.h>
#else
	#error"PageCache only supports windows!"
#endif // _WIN32


namespace DM
{

	class PageCache
	{
	public:
		~PageCache();
		static size_t PAGE_SIZE; // 4K页大小,操作系统页表大小大多为4kb
		static PageCache* Inst()
		{
			static PageCache instance;
			return &instance;
		}
		void*	AllocateSpan(size_t pageNum);//分配指定页数的span    
		void	DeallocateSpan(void* ptr, size_t pageNum);//释放span    
		void	ReturnMemoryToSystem();
	private:
		PageCache();
		/// <summary>
		/// 分配指定页数的内存块。
		/// </summary>
		/// <param name="pageNum">要分配的内存页数。</param>
		/// <returns>指向分配内存块的指针。如果分配失败，返回空指针（nullptr）。</returns>
		void*	SystemAlloc(size_t pageNum);
		/// <summary>
		/// 释放指定数量页面的内存。
		/// </summary>
		/// <param name="ptr">指向需要释放内存的起始地址的指针。</param>
		/// <param name="pageNum">要释放的内存页的数量。</param>
		void	SystemDeAlloc(void*ptr,size_t pageNum);
	private:
		struct Span
		{
			void* PageAddr;		//页起始地址        
			size_t NumPages;	//页数        
			Span* Next;			//链表指针
			~Span() = default;
		};
		std::map<size_t, Span*> _FreeSpans;	//按页数管理空闲span，不同页数对应不同Span链表    
		std::map<void*, Span*> _SpanMap;	
		std::mutex _Mutex;
		std::vector<std::pair<void*, size_t>>_SystemAllocatedMemory;
	};
}
