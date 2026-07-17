#include<DMPCH.h>
#include "Tool/Util/GameStatic.h"

#include<Resource/AssetManager.h>
namespace DM
{
	SPtr<Shader> GameStatic::GetShader(const std::string& name)
	{
		return AssetManager::GetInst()->GetAsset<Shader>(name);
	}
	SPtr<Texture2D> GameStatic::GetTexture2D(const std::string& name)
	{
		return AssetManager::GetInst()->GetAsset<Texture2D>(name);
	}
}


