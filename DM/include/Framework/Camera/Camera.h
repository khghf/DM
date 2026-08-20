#pragma once
#include<Foundation/Math/Vector.h>
#include<Foundation/Math/Matrix.h>
namespace DM
{
	class Event;
	enum class ECameraType
	{
		Ortho,
		Projection
	};
	class DM_API Camera
	{
	public:
		Camera();
		virtual void	OnUpdate(float deletaTime) = 0;
		virtual void	AddRotation(const Vector3& rotation)	{ m_Rotation += rotation;UpdateData(); }
		virtual void	AddPosition(const Vector3& offset)		{ m_Position += offset;UpdateData(); }
		
		virtual void	SetRotation(const Vector3& rotation)	{ m_Rotation = rotation;UpdateData(); }
		virtual void	SetPosition(const Vector3& position)	{ m_Position = position; UpdateData(); }
		virtual void	SetFOV(const float& fov)				{ m_FOV = fov;OnFovUpdate(); }
		virtual void	SetProjection(const float& aspectRatio)=0;
		virtual Vector3 GetRotation()const						{ return m_Rotation; }
		virtual Vector3 GetPosition()const						{ return m_Position; }
		virtual Vector3 GetForward()const						{ return m_Front; }
		virtual Vector3 GetRight()const							{ return m_Right; }
		virtual Vector3 GetUp()const							{ return m_Up; }
		virtual float	GetFov()const							{ return m_FOV; }
		const Matrix4&	GetProjectionMatrix()const				{ return m_ProjectionMatrix; }
		const Matrix4&	GetViewMatrix()const					{ return m_ViewMatrix; }
		const Matrix4&	GetProjectionViewMatrix()const			{ return m_ProjectionViewMatrix; }
	protected:
		virtual void OnFovUpdate() = 0;
		virtual void OnWindowResize(Event* const e) = 0;
	private:
		void UpdateData();
	protected:
		Matrix4 m_ProjectionMatrix;
		Matrix4 m_ViewMatrix;
		Matrix4 m_ProjectionViewMatrix;
		Vector3 m_Position;
		Vector3 m_Front;
		Vector3 m_Up;
		Vector3 m_Right;
		Vector3 m_Rotation;
		float m_FOV = 45.f;
		float m_AspectRatio;
	};
}
