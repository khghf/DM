#pragma once
#include"Texture.h"
#include<Core/Math/Vector.h>
namespace DM
{
	class DM_API SubTexture2D
	{
		SubTexture2D(const SPtr<Texture2D>& sourceTexture, const Vector2& cellSize, const Vector2& offset, const Vector2& cellCount = Vector2(1.f, 1.f));
	public:
		void SetCoord(float xCellOffset, float yCellOffset);
		void SetCoord(float xCellOffset, float yCellOffset, Vector2 cellSize);
		SPtr<Texture2D>GetSourceTexture()const { return m_Source; }
		const Vector2* GetCoord()const { return &m_Coord[0]; }
	private:
		SPtr<Texture2D>m_Source;
		Vector2 m_CellSize;
		Vector2 m_CellCount;
		Vector2 m_Coord[4];
	};
}
