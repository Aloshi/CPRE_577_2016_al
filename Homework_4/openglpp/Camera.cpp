#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Camera::Camera(float fov, float aspect, float near, float far)
{
	setPerspective(fov, aspect, near, far);
}

void Camera::setPerspective(float fovY, float aspect, float near, float far)
{
	mProjection = glm::perspective(fovY, aspect, near, far);
}

void Camera::lookAt(const glm::vec3& from, const glm::vec3& target, const glm::vec3& up)
{
	transform.setPosition(-from);
	
	glm::mat4 mat = glm::lookAt(from, target, up);
	transform.setRotation(glm::conjugate(glm::toQuat(mat)));
}