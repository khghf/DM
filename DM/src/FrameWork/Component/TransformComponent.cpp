#include<Framework/Component/TransformComponent.h>
#include<Foundation/Math/Quat.h>
namespace DM
{
	Matrix4 TransformComponent::GetTransform() const
	{
        glm::quat quaternion = glm::quat(glm::radians(Rotation));

        return  glm::translate(Matrix4(1.f), Location) *
                glm::mat4_cast(quaternion) *      
                glm::scale(Matrix4(1.f), Scale);
	}
}