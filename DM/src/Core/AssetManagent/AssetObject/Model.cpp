#include<Core/AssetManagent/AssetObject/Model.h>
#include<Core/RHI/RHI.h>
namespace DM
{
	Model::Model()
	{
	
	}
	Model::~Model()
	{
		if(m_Vertices)delete m_Vertices;
		if(m_Indices)delete m_Indices;
	}
}