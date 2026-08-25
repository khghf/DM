#include<Core/AssetManagent/AssetLoader/ModelLoader.h>
#include<Core/AssetManagent/AssetLoader/Texture2DLoader.h>
#include<Core/AssetManagent/AssetImporter/Texture2DImporter.h>
#include<Core/AssetManagent/AsetPack/ModelPack.h>
#include<Core/AssetManagent/AssetObject/Model.h>
#include<Core/AssetManagent/AssetObject/AssetObjectSetter.h>
#include<Core/RHI/RHI.h>
#include<Core/AssetManagent/AssetMetaDatabase.h>
#include<Core/AssetManagent/AssetCache.h>
namespace DM
{
	SPtr<AssetObject>ModelLoader::Load(std::string_view path)
	{
		ModelPack pack{};
		AssetUtil::Deserialize(&pack, path);
		return Load(&pack);
	}
	SPtr<AssetObject>ModelLoader::Load(AssetPack* pack)
	{

		Model* model = new Model();

		ModelPack* modelPack = static_cast<ModelPack*>(pack);

		//导入依赖资源

		if (modelPack->m_Textures.size() != modelPack->GetMeta().m_Dependency.size())
		{
			std::vector<SPtr<AssetObject>>temp(std::move(modelPack->m_Textures));//暂时引用已加载的资源防止被回收，这样可以避免二次加载相同的资源包

			modelPack->m_Textures.clear();
			const auto& metaInfo = modelPack->GetMeta();
			auto database = AssetMetaDatabase::Get();
			auto cache = AssetCache::Get();
			modelPack->m_Textures.reserve(metaInfo.m_Dependency.size());
			for (const auto& id : metaInfo.m_Dependency)
			{
				SPtr<AssetObject> asset = cache->GetAsset(id);

				if (asset)
				{
					modelPack->m_Textures.emplace_back(asset);
					continue;
				}

				std::string path = database->GetAssetPathByGuid(id);
				if (!path.empty())
				{
					asset = Texture2DLoader::Load(path);
					modelPack->m_Textures.emplace_back(asset);
					cache->CacheAsset(id, asset);
					continue;
				}

				DM_CORE_ASSERT(false, "{}", "Import dependency texture failed");
			}
		}



		auto* device = RHI::RHIDevice::Get();

		//std::vector<RHI::Vertex>rhiVertex{};
		//std::vector<uint32_t>rhiIndex{};

		//std::vector<RHI::VertexProperty>vertexPro{};
		//vertexPro.reserve(3);
		//for (const auto& mesh : modelPack->m_Meshs)
		//{
		//	for (const auto& vertex : mesh.vertices)
		//	{
		//		vertexPro.emplace_back(std::initializer_list{ vertex.Position.x, vertex.Position.y, vertex.Position.z });
		//		vertexPro.emplace_back(std::initializer_list{ vertex.Normal.x, vertex.Normal.y, vertex.Normal.z });
		//		vertexPro.emplace_back(std::initializer_list{ vertex.TexCoords.x, vertex.TexCoords.y });


		//		//这里踩的坑：由于顶点相关的结构内部使用的内存池分配内存，在对象析构时会还给内存池，这里扩容时会先构造新元素然后析构旧元素，由于我没有实现拷贝、移动构造函数
		//		//从而导致内存被意外回收，谨记对于使用内存池的类一定要实现相应构造函数转移资源
		//		rhiVertex.emplace_back(vertexPro);
		//		vertexPro.clear();
		//	}
		//	rhiIndex.insert(rhiIndex.end(), mesh.indices.begin(), mesh.indices.end());
		//}

		//RHI::VertexSet vSet{ std::move(rhiVertex) };
		//RHI::VertexLayout vertexLayout{
		//	{"inPosition",	RHI::EFormat::Float3},
		//	{"inNormal",	RHI::EFormat::Float3},
		//	{"inTexCoords",	RHI::EFormat::Float2},
		//};

		//RHI::RHIVertexBufferDesc vDesc{ vSet,vertexLayout };
		//model->m_Vertices = device->CreateVertexBuffer(vDesc);

		//RHI::RHIIndexBufferDesc indexDesc{ std::move(rhiIndex) };
		//model->m_Indices = device->CreateIndexBuffer(indexDesc);

		model->m_RefedTexture = std::move(modelPack->m_Textures);

		// 资产对象持有包内 GUID，后续可通过资产库反查源文件路径
		AssetObjectSetter::SetAssetId(model, pack->GetGUID());

		return CreateSPtr_Raw<Model>(model);

	}

	REGISTER_LOAD_PATH(EAssetType::Model, ModelLoader::Load);
	REGISTER_LOAD_PACK(EAssetType::Model, ModelLoader::Load);
}