#include<Foundation/GameStatic.h>
#include<Engine.h>
namespace DM
{
	WPtr<World> GameStatic::GetCurWorld()
	{
		return Engine::Get()->GetGameInst().m_CurWorld;
	}

}