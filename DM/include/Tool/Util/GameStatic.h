#pragma once
#include<string>
#include<Core/MMM/Reference.h>
namespace DM
{
	class Shader;
	class Texture2D;
	class DM_API GameStatic
	{
	public:
		static SPtr<Shader>			GetShader(const std::string& name);
		static SPtr<Texture2D>		GetTexture2D(const std::string& name);









		/*template<typename tTy,typename sTy>
		static bool	IsChildOf(sTy&&source)
		{
			using s1 = std::remove_reference_t<sTy>;
			using s2 = std::remove_pointer_t<s1>;
			using s3 = std::remove_cv_t<s2>;
			reflect::RClass* rclass = GetRClass(typeid(s3).hash_code());
			if (rclass == nullptr)return false;
			return rclass->_IsChildOf<tTy>();
		}*/
	};
}
