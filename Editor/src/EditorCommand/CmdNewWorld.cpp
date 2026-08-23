#include<EditorCommand/ICommand.h>
#include<Editor.h>
#include<Panel/ContentBrowserPanel.h>
#include<Core/AssetManagent/AssetImporter/WorldImporter.h>
#include<Core/AssetManagent/AsetPack/WorldPack.h>
#include<Core/AssetManagent/AssetMetaDatabase.h>
namespace DM
{
	static void NewWorld_Internal(const std::string& saveDir, std::string& temp, AssetPack*pack,int times);
	void CmdNewWorld::Execute()
	{
		std::string saveDir = static_cast<ContentBrowserPanel*>(Editor::Get()->GetContentBrowser())->GetCurPath();
		//在这踩的坑:使用new 在Editor.dll上分配World对象时、在Editor.dll本地生成了一份虚表(为什么会额外生成虚表而不是用的DM.dll的还没搞明白)
		//导致在序列化时根据虚表获取类型信息时无法获取正确的信息而报错
		saveDir;
		std::string savePath{};

		AssetPack* pack =static_cast<WorldPack*>(WorldImporter::Import(""));

		savePath = saveDir + "/NewWorld" + pack->GetExtension();


		NewWorld_Internal(saveDir, savePath, pack,1);
		const auto& record = AssetMetaDatabase::Get()->GetRecordByGuid(pack->GetGUID());
		m_Path = record->AssetPackPath;
		delete pack;
	}

	void CmdNewWorld::Undo()
	{
		AssetMetaDatabase::Get()->RemoveRecordBySourceFilePath(m_Path);
		AssetMetaDatabase::Get()->Save();
		FileSystem::DeleteFile_(m_Path);
	}

	void NewWorld_Internal(const std::string& saveDir, std::string& savePath, AssetPack* pack,int times)
	{
		if (std::filesystem::exists(savePath))
		{
			savePath = saveDir + "/NewWorld(" + std::to_string(times) +")"+ pack->GetExtension();
			NewWorld_Internal(saveDir, savePath, pack, ++times);
		}
		else
		{
			AssetMetaInfo metaInfo = pack->GetMeta();
			metaInfo.m_SourceFilePath = savePath;
			AssetPackSetter::SetMetaInfo(pack, metaInfo);
			WorldPack* worldPack = static_cast<WorldPack*>(pack);
			worldPack->m_WorldName = FileSystem::GetFileName(savePath);
			AssetUtil::SerializePack(pack, savePath);
		}
	}
}