#include"../include/GlobalData.h"
namespace mirror
{
	REFLECT_API GlobalData& mirror::GetGlobalData()
	{
		static GlobalData inst{};
		return inst;
	}
}

