#pragma once
#include<Foundation/MMM/Reference.h>
#include<Core/Core.h>
namespace DM
{
	class World;

	class DM_API GameStatic
	{
	public:
		static WPtr<World>GetCurWorld();
	};

}