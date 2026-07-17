#pragma once
#include"Texture.h"
#include<Core/Math/Vector.h>
namespace DM
{
	class DM_API TextureAtlas 
	{
	public:
		TextureAtlas(SPtr<Texture2D>atlas, Vector2 cellsize);
	private:
		SPtr<Texture2D>m_Atlas;
	};
}
