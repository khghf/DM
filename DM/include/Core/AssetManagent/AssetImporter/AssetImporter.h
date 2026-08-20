#pragma once
#include<string>
#include<vector>
#include<filesystem>
#include<stb_image.h>
#include<Core/Core.h>
namespace DM
{
	class AssetPack;
	
	struct DM_API AssetImporter
	{
		using Importer = AssetPack * (*)(std::string_view );

		/// <summary>
		/// 将外部文件导入并生成.dasset文件保存到磁盘、对于引擎内部资产类则直接生产资产包
		/// </summary>
		static AssetPack* Import(std::string_view sourceFilePath);
	private:
		static Importer SelectImporter(std::string_view sourceFilePath);
		/*static std::unordered_map<EAssetType, Importer>& GetImporterRegistry()
		{
			static std::unordered_map<EAssetType, Importer> registry;
			return registry;
		}*/
	public:
		/// <summary>
		/// 通过文件魔术头及扩展名判断是否为自定义资产，目前只通过扩展名判断并导入外部文件为置产
		/// 该函数暂时无用
		///  @todo
		/// </summary>
		/// <param name="sourceFilePath"></param>
		/// <returns></returns>
		static bool IsEngineAsset(std::string_view sourceFilePath);


	};


	//struct ImporterRegister
	//{
	//	ImporterRegister(EAssetType type, AssetImporter::Importer importer)
	//	{
	//		auto& registry = AssetImporter::GetImporterRegistry();
	//		registry[type] = importer;
	//	}
	//};

}
//#ifndef _CONCAT_
//#define _CONCAT_(a,b)a##b
//#endif // !_CONCAT_
//
//#define _REGISTER_IMPORTER(AssetType,Loader,ID)inline static DM::ImporterRegister _CONCAT_(register_,ID){AssetType,Loader};
//
//#define REGISTER_IMPORTER(AssetType,Loader)	_REGISTER_LOAD_PATH(AssetType,Loader,__COUNTER__)