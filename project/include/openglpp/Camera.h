#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Transform.h"

class Camera {
public:
	Camera(float fov, float aspect, float near = 0.01f, float far = 400.0f);

	void setPerspective(float fovY, float aspect, float near, float far);
	void lookAt(const glm::vec3& from, const glm::vec3& target, const glm::vec3& up = glm::vec3(0, 1, 0));

	inline const glm::mat4x4& projection() const { return mProjection; }
	inline const glm::mat4x4& view() const { return mView; }

	void setPosition(const glm::vec3& pos);
	inline const glm::vec3& position() const { return mPosition; }
	void setRotation(const glm::quat& rot);
	inline const glm::quat& rotation() const { return mRotation; }

	inline const glm::vec3& right() const { return mRight; }
	inline const glm::vec3& up() const { return mUp; }
	inline const glm::vec3& forward() const { return mForward; }

private:
	void rebuildView();
	void setView(const glm::mat4& mat);

	glm::mat4x4 mProjection;

	glm::vec3 mPosition;
	glm::quat mRotation;
	glm::mat4x4 mView;

	glm::vec3 mRight, mUp, mForward;
};
