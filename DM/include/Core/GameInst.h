#pragma once
#include<Core/Core.h>
#include<Foundation/MMM/Reference.h>
#include<Framework/Base/World.h>
namespace DM
{
	class World;
	class DM_API GameInst
	{
		friend class GameStatic;
	public:
		GameInst();
		~GameInst();
		void SetCurWorld(const SPtr<World>& world) { m_CurWorld = world; }
	private:
		SPtr<World>m_CurWorld;
	};
}