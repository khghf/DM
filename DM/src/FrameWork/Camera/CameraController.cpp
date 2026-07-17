#include "DMPCH.h"
#include <Framework/Camera/CameraController.h>
#include<Platform/Input/KeyCode.h>
#include<Platform/Input/Input.h>
#include<Core/EventSystem/Disptcher.h>
#include<Core/EventSystem/Event/MouseEvent.h>
namespace DM
{
	CameraController::CameraController()
	{
	}
	CameraController::CameraController(const float& aspectRatio, ECameraType type)
	{
		if (type == ECameraType::Ortho)
		{
			m_ViewPoint = std::make_shared<OrthoGraphicCamera>(aspectRatio);
		}
		else if (type == ECameraType::Projection)
		{

		}
	}
	
	CameraController::~CameraController()
	{
		
	}
	void CameraController::Control(SPtr<Camera> camera)
	{
		if (camera == nullptr)return;
		m_ViewPoint = camera;
	}
	void CameraController::OnUpdate(float deletaTime)
	{
		const float offset = m_MoveSpeed * m_MoveSensitivity*deletaTime;
		Vector3 pos = GetPosition();
		if (Input::s_Instance && Input::s_Instance->IsKeyPressed(KeyCode::W))pos += offset * GetUp();
		if (Input::s_Instance && Input::s_Instance->IsKeyPressed(KeyCode::S))pos -= offset * GetUp();
		if (Input::s_Instance && Input::s_Instance->IsKeyPressed(KeyCode::A))pos -= offset * GetRight();
		if (Input::s_Instance && Input::s_Instance->IsKeyPressed(KeyCode::D))pos += offset * GetRight();
		SetPosition(pos);
		m_ViewPoint->OnUpdate(deletaTime);
	}

	void CameraController::OnEvent(Event* const e)
	{
		Disptcher dis(e);
		dis.DisptchSpecifiedEvent<MouseScroll>([this](Event* const e) {
			this->OnMouseScroll(e);
			});
	}

	void CameraController::OnMouseScroll(Event const* e)
	{
		LOG_CORE_INFO("CameraController.OnMouseScroll");
		const float fov = GetFov()-e->GetData<MouseScroll>()->offset.y*m_ZoomSensitivity;
		SetFOV(fov);
	}
}
