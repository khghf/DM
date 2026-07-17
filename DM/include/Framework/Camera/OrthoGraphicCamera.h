#pragma once
#include"Camera.h"
namespace DM
{
	class DM_API OrthoGraphicCamera:public Camera
	{
	public:
		OrthoGraphicCamera(float aspectRatio);
		virtual void OnUpdate(float reletaTime) override;
		virtual void SetProjection(const float& aspectRatio) override;
	protected:
		virtual void OnFovUpdate() override;
		virtual void OnWindowResize(Event* const e) override;

	};
}
