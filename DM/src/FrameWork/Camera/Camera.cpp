#include "DMPCH.h"
#include<Framework/Camera/Camera.h>
#include<glm/gtc/matrix_transform.hpp>
#include<Core/EventSystem/EventMgr.h>
#include<Core/EventSystem/Event/WindowEvent.h>
namespace DM
{
	Camera::Camera():
		m_ProjectionMatrix(Matrix4(1.f)), m_ViewMatrix(Matrix4(1.f)), 
		m_ProjectionViewMatrix(Matrix4(1.f)), m_Position(Vector3(0.f)),
		m_Rotation(Vector3(0.f, -90.f, 0.f))
	{
		EventMgr::GetInst()->Register<WindowResize>([this](Event* const e) {this->OnWindowResize(e); });
	}
	void Camera::UpdateData()
	{
		const float& pitch = m_Rotation.x;
		const float& yaw = m_Rotation.y;
		const float& roll = m_Rotation.z;

		m_Front.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
		m_Front.y = glm::sin(glm::radians(pitch)) * glm::sin(glm::radians(roll));
		m_Front.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
		m_Front = glm::normalize(m_Front);

		m_Right = glm::cross(m_Front, Vector3(0.f, 1.f, 0.f));
		m_Right = glm::normalize(m_Right);

		m_Up = glm::cross(m_Right, m_Front);
		m_Up = glm::normalize(m_Up);

		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
		m_ProjectionViewMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}

