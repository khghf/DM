#pragma once
#include"OrthoGraphicCamera.h"
#include<Foundation/MMM/Reference.h>
namespace DM
{
	class Event;
	class DM_API CameraController
	{
	public:
		CameraController();
		CameraController(const float&aspectRatio,ECameraType type);
		~CameraController();
		void Control(SPtr<Camera> camera);
		void OnUpdate(float deletaTime);
		void HandleEvent(Event* const e);

		void		AddRotation(const Vector3& rotation)						{ GetCamera()->AddRotation(rotation); }
		void		AddPosition(const Vector3& offset)							{ GetCamera()->AddPosition(offset); }
		void		AddRotation(float pitch, float yaw, float roll)				{ AddRotation(Vector3(pitch, yaw, roll)); }
		void		AddPosition(float xOffset, float yOffset, float zOffset)	{ AddPosition(Vector3(xOffset, yOffset, zOffset)); }


		SPtr<Camera> GetCamera()const											{ return m_ViewPoint; }
		Vector3		GetPosition()const											{ return GetCamera()->GetPosition(); }
		Vector3		GetRotation()const											{ return GetCamera()->GetRotation(); }
		Vector3		GetForward()const											{ return GetCamera()->GetForward(); }
		Vector3		GetRight()const												{ return GetCamera()->GetRight(); }
		Vector3		GetUp()const												{ return GetCamera()->GetUp(); }
		float		GetFov()const												{ return GetCamera()->GetFov(); }
		

		void		SetPosition(const Vector3& position)						{ GetCamera()->SetPosition(position); }
		void		SetRotation(const Vector3& rotation)						{ GetCamera()->SetRotation(rotation); }
		void		SetRotation(float pitch, float yaw, float roll)				{ SetRotation(Vector3(pitch, yaw, roll)); }
		void		SetPosition(float x, float y, float z)						{ SetPosition(Vector3(x, y, z)); }
		void		SetFOV(const float&fov)										{ GetCamera()->SetFOV(fov); }
		void		SetProjection(const float& aspectRatio)						{ GetCamera()->SetProjection(aspectRatio); }
	protected:
		void OnMouseScroll(Event const* e);
	public:
		float m_MoveSensitivity = 1.f;
		float m_RotationSensitivity = 1.f;
		float m_ZoomSensitivity = 5.f;
		float m_MoveSpeed = 1.f;
		float m_RotationSpeed = 1.f;
	private:
		float m_ZoomLevel = 1.f;
		SPtr<Camera>m_ViewPoint;
	};
}
