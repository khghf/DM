#include<Core/AssetManagent/AssetImporter/ModelImporter.h>
#include<Core/AssetManagent/AssetImporter/Texture2DImporter.h>
#include<Core/AssetManagent/AssetLoader/Texture2DLoader.h>
#include<Core/AssetManagent/AsetPack/ModelPack.h>
#include<Core/AssetManagent/AsetPack/TexturePack.h>
#include<Core/AssetManagent/AssetCache.h>
#include<Core/AssetManagent/AssetMetaDatabase.h>
#include<assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include<filesystem>


namespace DM
{
	static Vector3 aiVectorToVector3(const aiVector3D& vec)
	{
		return Vector3(vec.x, vec.y, vec.z);
	}
	static Vector2 aiVectorToVector2(const aiVector3D& vec)
	{
		return Vector2(vec.x, vec.y);
	}

	static std::vector<AssetID> loadMaterialTextures(aiMaterial* mat, aiTextureType type, AssetMetaInfo& metaInfo, std::vector<TexturePack*>& loadedPacks)
	{
		std::vector<AssetID>textureId;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);

			std::string texturePath = std::filesystem::path{ metaInfo.m_SourceFilePath }.parent_path().string() + "/" + str.C_Str();

			bool skip = false;

			for (TexturePack* p : loadedPacks)
			{
				std::filesystem::path p1{ texturePath };
				std::filesystem::path p2{ p->GetMeta().m_SourceFilePath };

				if (p1.compare(p2) == 0)
				{
					skip = true;
					textureId.emplace_back(p->GetGUID());
					break;
				}
			}

			if (!skip)
			{
				//导入依赖的纹理资源
				AssetPack* pack = Texture2DImporter::Import(texturePath);

				metaInfo.m_Dependency.emplace_back(pack->GetGUID());

				std::string_view sourceFilePath{ texturePath };
				std::string packPath=AssetUtil::SerializePack(pack, sourceFilePath);

				AssetMetaDatabase::Get()->AddNewAssetPack(pack->GetGUID(), packPath, pack->GetMeta().m_SourceFilePath, pack->GetMeta().m_SourceFileContentHash, pack->GetMeta().m_SourceFileLastModifyTime, pack->GetAssetType());

				loadedPacks.emplace_back(static_cast<TexturePack*>(pack));
				textureId.emplace_back(pack->GetGUID());
			}
		}
		return textureId;
	}

	static Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, AssetMetaInfo& metaInfo, std::vector<TexturePack*>& dependencyPack)
	{
		//提取位置、法线、切线、纹理坐标数据
		std::vector<MeshVertex> vertices;
		vertices.reserve(mesh->mNumVertices);

		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
		{
			MeshVertex vertex{};
			// positions
			vertex.Position = aiVectorToVector3(mesh->mVertices[i]);
			// normals
			if (mesh->HasNormals())
			{
				vertex.Normal = aiVectorToVector3(mesh->mNormals[i]);
			}
			// texture coordinates
			if (mesh->mTextureCoords[0])
			{
				//一个顶点可能包含多个通道的纹理坐标，这里暂时只取第一个通道的
				vertex.TexCoords = aiVectorToVector2(mesh->mTextureCoords[0][i]);
				// tangent
				vertex.Tangent = aiVectorToVector3(mesh->mTangents[i]);
				// bitangent
				vertex.Bitangent = aiVectorToVector3(mesh->mBitangents[i]);
			}
			else
			{
				vertex.TexCoords = Vector2(0.0f, 0.0f);
			}
			vertices.emplace_back(vertex);
		}

		//提取顶点索引数据
		std::vector<unsigned int> indices;
		indices.reserve(mesh->mNumFaces * 10);
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j)indices.emplace_back(face.mIndices[j]);
		}

		//提取纹理数据
		std::vector<AssetID> textures;

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];


		std::vector<AssetID>diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, metaInfo, dependencyPack);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<AssetID>specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, metaInfo, dependencyPack);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<AssetID>normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, metaInfo, dependencyPack);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<AssetID>heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, metaInfo, dependencyPack);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		return Mesh{ vertices,indices,textures };

	}


	static void ProcessNode(aiNode* node, const aiScene* scene, AssetMetaInfo& metaInfo, ModelPack* pack, std::vector<TexturePack*>& dependencyPack)
	{
		//处理该节点的网格
		for (unsigned int i = 0; i < node->mNumMeshes; ++i)
		{
			//结点只包含网格索引
			unsigned int meshIndex = node->mMeshes[i];
			aiMesh* mesh = scene->mMeshes[meshIndex];

			Mesh m = ProcessMesh(mesh, scene, metaInfo, dependencyPack);
			pack->m_Meshs.emplace_back(std::move(m));
		}
		// 递归处理子节点的网格
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene, metaInfo, pack, dependencyPack);
		}
	}

	

	









	AssetPack* ModelImporter::Import(std::string_view assetPath)
	{
		ModelPack* pack = new ModelPack();

		AssetMetaInfo metaInfo = pack->GenerateMetaInfo(assetPath);


		Assimp::Importer importer;
		std::string filePath{ assetPath.data() };
		const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		DM_CORE_ASSERT(scene, "{}", "[struct ModelImporter]import asset failed");

		std::vector<TexturePack*>dependencyPack;

		ProcessNode(scene->mRootNode, scene, metaInfo, pack, dependencyPack);

		for (const auto& mesh : pack->m_Meshs)
		{
			metaInfo.m_DataSize += mesh.vertices.size() * sizeof(MeshVertex);
			metaInfo.m_DataSize += mesh.indices.size() * sizeof(unsigned int);
		}
		

		AssetPackSetter::SetGuid(pack, AssetID::GenNewID());
		AssetPackSetter::SetMetaInfo(pack, metaInfo);

		pack->m_Textures.reserve(dependencyPack.size());



		AssetMetaDatabase* database = AssetMetaDatabase::Get();
		AssetCache* cache = AssetCache::Get();

		//加载依赖纹理
		for (TexturePack* texPack : dependencyPack)
		{
			SPtr<AssetObject>tex = Texture2DLoader::Load(texPack);

			pack->m_Textures.emplace_back(tex);
			

			cache->CacheAsset(texPack->GetGUID(), tex);
		}
		database->Save();
		return pack;

	}


}