#pragma once
#include"AssetImporter.h"
namespace DM
{
	struct DM_API WorldImporter:AssetImporter
	{
		/// <summary>
		/// 从指定的源文件路径导入资源包。world无需依赖外部文件即可创建，当传入空路径时会生成一个默认世界资产包
		/// </summary>
		/// <param name="sourceFilePath">资源文件的路径，表示要导入的文件位置。</param>
		/// <returns>指向导入的资源包的指针。如果导入失败，可能返回空指针。</returns>
		static AssetPack* Import(std::string_view sourceFilePath);
	};
}