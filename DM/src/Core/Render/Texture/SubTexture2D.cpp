#include "DMPCH.h"
#include "Core/Render/Texture/SubTexture2D.h"
namespace DM
{
	SubTexture2D::SubTexture2D(const SPtr<Texture2D>& sourceTexture, const Vector2& cellSize, const Vector2& offset, const Vector2& cellCount):
		m_Source(sourceTexture),m_CellSize(cellSize), m_CellCount(cellCount)
	{
		SetCoord(offset.x, offset.y);
	}
	void SubTexture2D::SetCoord(float xCellOffset, float yCellOffset)
	{
		m_Coord[0].x = xCellOffset * m_CellSize.x; m_Coord[0].y = yCellOffset * m_CellSize.y;
		m_Coord[1].x = m_Coord[0].x; m_Coord[1].y = m_Coord[0].y + (m_CellCount.y * m_CellSize.y / m_Source->GetHeight());
		m_Coord[2].x = m_Coord[1].x + (m_CellCount.x * m_CellSize.x / m_Source->GetWidth()); m_Coord[2].y = m_Coord[1].y;
		m_Coord[3].x = m_Coord[2].x; m_Coord[3].y = m_Coord[0].y;
	}

	void SubTexture2D::SetCoord(float xCellOffset, float yCellOffset, Vector2 cellSize)
	{
		m_CellSize = cellSize;
		SetCoord(xCellOffset, yCellOffset);
	}
}


