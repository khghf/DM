#pragma once
#include"../AssetObject/Texture2D.h"
#include<Foundation/Math/Vector.h>
#define MAX_BONE_INFLUENCE 4
namespace DM
{

	struct MeshVertex
	{

		Vector3 Position;
		Vector3 Normal;
		Vector2 TexCoords;
		Vector3 Tangent;
		Vector3 Bitangent;
		//影响顶点的骨骼
		int m_BoneIDs[MAX_BONE_INFLUENCE];
		//骨骼权重
		float m_Weights[MAX_BONE_INFLUENCE];
	};
	REGISTER_MEMBER(MeshVertex, Position);
	REGISTER_MEMBER(MeshVertex, Normal);
	REGISTER_MEMBER(MeshVertex, TexCoords);
	REGISTER_MEMBER(MeshVertex, Tangent);
	REGISTER_MEMBER(MeshVertex, Bitangent);



	struct Mesh
	{
		std::vector<MeshVertex>     vertices;
		std::vector<unsigned int>	indices;
		std::vector<AssetID>		textures;//网格的纹理资源id
		Mesh() {}
		Mesh(std::vector<MeshVertex> vertices, std::vector<unsigned int> indices, std::vector<AssetID> textures)
		{
			this->vertices = vertices;
			this->indices = indices;
			this->textures = textures;
		}
	};
	REGISTER_MEMBER(Mesh, vertices);
	REGISTER_MEMBER(Mesh, indices);
	REGISTER_MEMBER(Mesh, textures);

	struct DM_API ModelPack :public AssetPack
	{
		virtual EAssetType GetAssetType() const override { return EAssetType::Model; }

		std::vector<Mesh>m_Meshs;
		std::vector<SPtr<AssetObject>>m_Textures;
	};
	REGISTER_CHILD(AssetPack, ModelPack);
	REGISTER_MEMBER(ModelPack, m_Meshs);
}