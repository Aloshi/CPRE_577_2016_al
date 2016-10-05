#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float fov, float aspect, float near, float far)
	: mTranslation(0, 0, 0), mScale(1.0f, 1.0f, 1.0f)
{
	setPerspective(fov, aspect, near, far);
}

void Camera::rebuildMatrix()
{
	// TODO Scale might be in the wrong place
	mMatrix = glm::translate(glm::mat4(1.0f), mTranslation) * glm::mat4_cast(mRotation) * glm::scale(glm::mat4(1.0f), mScale);
	mRight = mRotation * glm::vec3(1, 0, 0);
	mUp = mRotation * glm::vec3(0, 1, 0);
	mForward = mRotation * glm::vec3(0, 0, 1);
}

void Camera::setPerspective(float fovY, float aspect, float near, float far)
{
	mProjection = glm::perspective(fovY, aspect, near, far);
}

void Camera::setPosition(const glm::vec3& pos)
{
	mTranslation = pos;
	rebuildMatrix();
}

void Camera::setRotation(const glm::quat& rot)
{
	mRotation = rot;
	rebuildMatrix();
}

void Camera::lookAt(const glm::vec3& from, const glm::vec3& target, const glm::vec3& up)
{
	mTranslation = from;
	
	glm::mat4 mat = glm::lookAt(from, target, up);
	mRotation = glm::conjugate(glm::quat(mat));

	rebuildMatrix();
}