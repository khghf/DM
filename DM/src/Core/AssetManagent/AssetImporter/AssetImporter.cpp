#include<Core/AssetManagent/AssetImporter/AssetImporter.h>
#include<Core/AssetManagent/AssetImporter/Texture2DImporter.h>
#include<Core/AssetManagent/AssetImporter/ModelImporter.h>
#include<Core/AssetManagent/AssetImporter/ShaderImporter.h>
#include<Core/AssetManagent/AssetImporter/WorldImporter.h>
#include<unordered_map>
#include<Core/AssetManagent/AssetFileDetector.h>
#include<Core/Log.h>
#include<Core/AssetManagent/AssetMetaDatabase.h>
namespace DM
{
    static std::unordered_map<std::string, AssetImporter::Importer>ImportableAsset =
    {
        {".jpg",Texture2DImporter::Import},
        {".png",Texture2DImporter::Import},

        {".fbx",ModelImporter::Import},
        {".glb",ModelImporter::Import},
        {".dae",ModelImporter::Import},
        {".3ds",ModelImporter::Import},
        {".obj",ModelImporter::Import},
        {".stl",ModelImporter::Import},
        {".gltf",ModelImporter::Import},


        {".vert",ShaderImporter::Import},
        {".frag",ShaderImporter::Import},
        {".geom",ShaderImporter::Import},
        {".comp",ShaderImporter::Import},
        {".tecs",ShaderImporter::Import},
        {".tese",ShaderImporter::Import},

        {".world",WorldImporter::Import},
    };
    AssetPack* AssetImporter::Import(std::string_view sourceFilePath)
    {
        AssetImporter::Importer importer = SelectImporter(sourceFilePath);
        if (!importer)
        {
            LOG_CORE_WARN("{}", "Import asset failed. unsupport asset format");
        }
        return importer?importer(sourceFilePath):nullptr;
    }
   
    AssetImporter::Importer AssetImporter::SelectImporter(std::string_view sourceFilePath)
	{
        
        std::string extension;
        if (const MagicHeaderInfo* info = AssetFileDetector::DetectByMagic(sourceFilePath.data()))
        {
            extension = info->Extension;
        }
        else
        {
            extension = AssetUtil::GetExtension(sourceFilePath);
        }
        auto it = ImportableAsset.find(extension);

        if (it != ImportableAsset.end())return  it->second;

        DM_CORE_ASSERT(false,"{}", "Unsupport asset format");
        return nullptr;
	}

    bool AssetImporter::IsEngineAsset(std::string_view sourceFilePath)
    {
        return false;
    }
}