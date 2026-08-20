#pragma once
#include<Foundation/MMM/Reference.h>
namespace DM
{
	class World;
	struct WorldContext
	{
		SPtr<World>m_CurWorld;
	};
}