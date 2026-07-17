#include "DMPCH.h"
#include "Core/Serializer.h"
#include<yaml-cpp/yaml.h>
#include<fstream>
#include<Tool/Util/PlatformUtils.h>
#include"DM.h"
namespace YAML
{
	YAML::Emitter& operator << (YAML::Emitter& out, const DM::Vector3& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}
	YAML::Emitter& operator << (YAML::Emitter& out, const DM::Vector4 v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w<<YAML::EndSeq;
		return out;
	}
	template<>
	struct convert<DM::Vector3>
	{
		static bool decode(const Node& node, DM::Vector3& vec)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			vec.x = node[0].as<float>();
			vec.y = node[1].as<float>();
			vec.z = node[2].as<float>();

			return true;
		}

		static Node encode(const DM::Vector3& vec)
		{
			Node node;
			node.push_back(vec.x);
			node.push_back(vec.y);
			node.push_back(vec.z);
			node.SetStyle(YAML::EmitterStyle::Flow);
			return node;
		}
	};


	template<>
	struct convert<DM::Vector4>
	{
		static bool decode(const Node& node, DM::Vector4& vec)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;
			vec.x = node[0].as<float>();
			vec.y = node[1].as<float>();
			vec.z = node[2].as<float>();
			vec.w = node[3].as<float>();
			return true;
		}

		static Node encode(const DM::Vector4& vec)
		{
			Node node;
			node.push_back(vec.x);
			node.push_back(vec.y);
			node.push_back(vec.z);
			node.push_back(vec.w);
			node.SetStyle(YAML::EmitterStyle::Flow);
			return node;
		}
	};
}

namespace DM
{
	namespace fs = std::filesystem;

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap; 
		{
			out << YAML::Key << "ID" << YAML::Value << 11111111111;
			out << YAML::Key << "Components" << YAML::Value;
			out << YAML::BeginMap; 
			{
				if (entity.HasComponent<TagComponent>())
				{
					auto com = entity.GetComponent<TagComponent>();
					out << YAML::Key << "TagComponent" << YAML::Value;
					out << YAML::BeginMap; 
					{
						out << YAML::Key << "Tag" << YAML::Value << com->Tag; 
					}
					out << YAML::EndMap;
				}

				if (entity.HasComponent<SpriteComponent>())
				{
					auto com = entity.GetComponent<SpriteComponent>();
					out << YAML::Key << "SpriteComponent" << YAML::Value;
					out << YAML::BeginMap; 
					{
						out << YAML::Key << "Color" << YAML::Value<<com->Color;
					}
					out << YAML::EndMap;
				}

				if (entity.HasComponent<TransformComponent>())
				{
					auto com = entity.GetComponent<TransformComponent>();
					out << YAML::Key << "TransformComponent" << YAML::Value;
					out << YAML::BeginMap; 
					{
						out << YAML::Key << "Location" << YAML::Value<< com->Location;

						out << YAML::Key << "Rotation" << YAML::Value<< com->Rotation;

						out << YAML::Key << "Scale" << YAML::Value<< com->Scale;
					}
					out << YAML::EndMap;
				}
			}
			out << YAML::EndMap; 
		}
		out << YAML::EndMap; 
	}

	void Serializer::Serialize(const SPtr<World>& scene)
	{
		if (!scene->bHasSavedToLocal)
		{
			const std::string path = FileDialog::SaveFile(scene->m_Name.c_str());
			scene->m_path = path+"."+scene->s_FileExtension.data();
		}
		fs::path pa(scene->m_path);
		std::ofstream outfile(pa.string());
		scene->m_Name = pa.filename().string();
		if (!(fs::exists(pa) && fs::is_regular_file(pa)))
		{
			DM_CORE_ASSERT(false, "{} is invalid path!", pa.string());
		}
		YAML::Emitter out;
		out.SetIndent(4);
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Scene" << YAML::Value << scene->m_Name;
			out << YAML::Key << "Entities" << YAML::Value;
			out << YAML::BeginSeq;
			{
				const auto& view = scene->m_Registry.view<entt::entity>();
				for (auto& entity : view)
				{
					if (!scene->m_Registry.valid(entity))continue;
					Entity en{ entity,scene.get() };
					SerializeEntity(out, en);
				}
			}
			out << YAML::EndSeq;
		}
		out << YAML::EndMap;
		outfile << out.c_str();
	}

	SPtr<World> Serializer::DeSerialize(const std::filesystem::path& filepath)
	{
		YAML::Node node = YAML::LoadFile(filepath.string());
		if (!node["Scene"])return nullptr;
		SPtr<World>scene = CreateSPtr<World>(new World());
		scene->m_path = filepath.string();
		scene->m_Name = filepath.stem().string();
		scene->bHasSavedToLocal = true;
		const std::string& sceneName = node["Scene"].as<std::string>();
		YAML::Node entities = node["Entities"];
		if (!entities.IsSequence())
		{
			std::cerr << "Error: 'Entities' node is not a sequence (invalid YAML format)." << std::endl;
			return nullptr;
		}

		if (entities)
		{
			for (YAML::Node entity : entities)
			{
				auto en = scene->CreateEntity();
				YAML::Node components = entity["Components"];
				if (YAML::Node com = components["TagComponent"])
				{
					en.GetComponent<TagComponent>()->Tag = com["Tag"].as<std::string>();
				}
				if (YAML::Node com = components["SpriteComponent"])
				{
					en.GetComponent<SpriteComponent>()->Color = com["Color"].as<Vector4>();
				}
				if (YAML::Node com = components["TransformComponent"])
				{
					en.GetComponent<TransformComponent>()->Location = com["Location"].as<Vector3>();
					en.GetComponent<TransformComponent>()->Rotation = com["Rotation"].as<Vector3>();
					en.GetComponent<TransformComponent>()->Scale = com["Scale"].as<Vector3>();
				}
			}
		}
		return scene;
	}

}

