#pragma once
#include<Core/Reflection/Mirror/include/ReflectMarco.h>
#include<Core/Reflection/Mirror/include/mirror.h>
#include<entt/entt.hpp>
#include<Foundation/MMM/Reference.h>
#include<unordered_set>
#include<Core/Log.h>
namespace DM
{
	class DM_API Object
	{
	public:
		Object();
		virtual~Object()=default;
	public:
		virtual void Update(float DeltaTime);
		virtual void Destroy();
		virtual void OnDestroy();

		inline void EnableUpdate(const bool& bEnable) { bEnableUpdate = bEnable; }
		inline bool IsEnableUpdate()const { return bEnableUpdate; }
	private:
			bool bEnableUpdate;
	};
}
