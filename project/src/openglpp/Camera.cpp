#include <openglpp/Camera.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include <assert.h>

Camera::Camera(float fov, float aspect, float near, float far)
{
	setPerspective(fov, aspect, near, far);
}

void Camera::setPerspective(float fovY, float aspect, float near, float far)
{
	mProjection = glm::perspective(fovY, aspect, near, far);
}

void Camera::setPosition(const glm::vec3& pos)
{
	mPosition = pos;
	rebuildView();
}

void Camera::setRotation(const glm::quat& rot)
{
	mRotation = rot;
	rebuildView();
}

void Camera::rebuildView()
{
	glm::mat4 translation = glm::translate(mPosition);
	glm::mat4 rotation = glm::mat4_cast(mRotation);
	mView = rotation * translation;

	mRight = glm::vec3(1, 0, 0) * mRotation;
	mUp = glm::vec3(0, 1, 0) * mRotation;
	mForward = glm::vec3(0, 0, 1) * mRotation;
}

void Camera::setView(const glm::mat4& mat)
{
	mView = mat;
	mPosition = glm::vec3(mat[3][0], mat[3][1], mat[3][2]);
	mRotation = glm::conjugate(glm::toQuat(mat));

	mRight = mRotation * glm::vec3(1, 0, 0);
	mUp = mRotation * glm::vec3(0, 1, 0);
	mForward = mRotation * glm::vec3(0, 0, 1);
}

void Camera::lookAt(const glm::vec3& from, const glm::vec3& target, const glm::vec3& up)
{
	setView(glm::lookAt(from, target, up));
}

glm::vec3 Camera::screenToWorld(float x, float y, float depth) const
{
	assert(x >= -1 && x <= 1 && y >= -1 && y <= 1 && depth >= -1 && depth <= 1);

	glm::mat4 mat = glm::inverse(mProjection * mView);
	glm::vec4 vec(x, y, depth, 1.0f);

	glm::vec4 pos = vec * mat;
	pos.w = 1.0f / pos.w;
	pos.x *= pos.w;
	pos.y *= pos.w;
	pos.z *= pos.w;
	return glm::vec3(pos);
}

