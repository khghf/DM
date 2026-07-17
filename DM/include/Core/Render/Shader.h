#pragma once
#include<DMPCH.h>
#include<Resource/Asset.h>
#include<Core/MMM/Reference.h>
#include<Core/Math/Vector.h>
#include<Core/Math/Matrix.h>
namespace DM
{
	class DM_API Shader:public Asset
	{
	public:
		Shader();
		virtual ~Shader();
		virtual void Bind() = 0;
		virtual void UnBind() = 0;

		virtual void SetInt(const std::string_view& name, const int32_t& val) = 0;
		virtual void SetIntArray(std::string_view name, const int32_t* val, const uint32_t& count) = 0;
		virtual void SetFloat3(const std::string_view& name, const Vector3& val)=0;
		virtual void SetFloat4(const std::string_view& name, const Vector4& val)=0;
		virtual void SetMat3x3(const std::string_view& name, const Matrix3& val)=0;
		virtual void SetMat4x4(const std::string_view& name, const Matrix4& val)=0;

		static SPtr<Shader> Create(const std::string_view&name,const std::string_view& vsCode, const std::string_view& fsCode);
		static SPtr<Shader> Create(const std::string_view& glslCodePath);
	public:
		constexpr static EAssetsCategory s_AssetsCategory = EAssetsCategory::Shader;
	private:
	};
}
