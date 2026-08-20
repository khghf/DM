#pragma once
#include<Core/AssetManagent/AssetObject/Texture2D.h>
#include<Core/RHI/RHI.h>
namespace DM
{
	Texture2D::Texture2D()
	{

	}
	Texture2D::~Texture2D()
	{
		if (m_RHITexture)delete m_RHITexture;
	}
}