#pragma once
#include<Core/Reflection/Mirror/ReflectMarco.h>
#include<Core/Reflection/Mirror/mirror.h>
#include<Tool/Util/GameStatic.h>
#include<entt/entt.hpp>
#include<Core/MMM/Reference.h>
#include<unordered_set>
#include<Core/Log.h>
namespace DM
{
	class Component;
	class DM_API CLASS() Object
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
			SPtr<int>test;
			bool bEnableUpdate;
	};
}
