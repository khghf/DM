#include<Core/AssetManagent/AssetImporter/AssetImporter.h>
#include<Core/AssetManagent/AssetImporter/Texture2DImporter.h>
#include<Core/AssetManagent/AssetImporter/ModelImporter.h>
#include<Core/AssetManagent/AssetImporter/ShaderImporter.h>
#include<Core/AssetManagent/AssetImporter/WorldImporter.h>
#include<unordered_map>
#include<Core/AssetManagent/AssetFileDetector.h>
#include<Core/Log.h>
#include<Core/AssetManagent/AssetMetaDatabase.h>
#include<cctype>
#include<algorithm>
namespace DM
{
    namespace
    {
        /// <summary>
        /// 模型附属文件：.mtl 是 .obj 的材质定义文件，随模型导入时由 assimp 一并读取，
        /// 不应被当作独立资产导入。需在魔数检测前拦截——OBJ 的魔数(0x23 0x20 "# ")与
        /// .mtl 的注释行头相同，会导致 .mtl 被误判为 .obj。
        /// </summary>
        bool IsModelSidecarFile(const std::string& extension)
        {
            std::string lower = extension;
            std::transform(lower.begin(), lower.end(), lower.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            return lower == ".mtl";
        }
    }
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
        return Import(sourceFilePath, true);
    }

    AssetPack* AssetImporter::Import(std::string_view sourceFilePath, bool reuseGuid)
    {
        AssetImporter::Importer importer = SelectImporter(sourceFilePath);
        if (!importer)
        {
            LOG_CORE_WARN("{}", "Import asset failed. unsupport asset format");
            return nullptr;
        }

        AssetPack* pack = importer(sourceFilePath);
        if (!pack) return nullptr;

        // 无论导入器是"生成式"(Texture/Model/Shader)还是"反序列化式"(World 会从文件
        // 带回旧的 m_SourceFilePath)，统一将元数据指向本次导入的目标路径。
        // 否则复制粘贴出的新文件会带着旧路径注册，被 AddNewAssetPack 的路径去重跳过，
        // 导致新资产不注册、内容浏览器不刷新。
        AssetPackSetter::SetMetaInfo(pack, pack->GenerateMetaInfo(sourceFilePath));

        // 复用已有 GUID：同一源文件重复导入(内容变更重导入、重命名/移动且内容未变)时
        // 保持资产身份稳定，避免场景等处的引用因 GUID 变化而失效。
        // 注意：这里只依赖 AssetMetaDatabase 做身份解析，不反向依赖 AssetMgr，
        // 保证"导入(编辑器侧) -> 加载(AssetMgr)"的单向依赖。
        if (reuseGuid && !sourceFilePath.empty())
        {
            AssetID existingId = AssetMetaDatabase::Get()->ResolveAssetIDBySourcePath(sourceFilePath);
            if (existingId.IsValid())
            {
                AssetPackSetter::SetGuid(pack, existingId);
            }
        }
        return pack;
    }
   
    AssetImporter::Importer AssetImporter::SelectImporter(std::string_view sourceFilePath)
	{
        const std::string extension = AssetUtil::GetExtension(sourceFilePath);

        // 扩展名优先：明确属于不可导入的附属格式直接拒绝
        if (IsModelSidecarFile(extension)) return nullptr;

        // 扩展名已知 -> 直接使用对应导入器
        auto it = ImportableAsset.find(extension);
        if (it != ImportableAsset.end()) return it->second;

        // 扩展名未知时用魔数兜底，兼容无扩展名或扩展名不正确的文件
        if (const MagicHeaderInfo* info = AssetFileDetector::DetectByMagic(sourceFilePath.data()))
        {
            auto magicIt = ImportableAsset.find(info->Extension);
            if (magicIt != ImportableAsset.end()) return magicIt->second;
        }

        DM_CORE_ASSERT(false,"{}", "Unsupport asset format");
        return nullptr;
	}

    bool AssetImporter::IsEngineAsset(std::string_view sourceFilePath)
    {
        return false;
    }

    bool AssetImporter::IsImportable(std::string_view sourceFilePath)
    {
        const std::string extension = AssetUtil::GetExtension(sourceFilePath);

        // 附属格式直接排除
        if (IsModelSidecarFile(extension)) return false;

        // 扩展名已知->可导入
        if (ImportableAsset.find(extension) != ImportableAsset.end()) return true;

        // 扩展名未知时用魔数兜底
        if (const MagicHeaderInfo* info = AssetFileDetector::DetectByMagic(sourceFilePath.data()))
        {
            return ImportableAsset.find(info->Extension) != ImportableAsset.end();
        }
        return false;
    }
}