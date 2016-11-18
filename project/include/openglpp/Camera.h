#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Transform.h"

class Camera {
public:
	Camera(float fov, float aspect, float near = 0.01f, float far = 100.0f);

	Transform transform;

	void setPerspective(float fovY, float aspect, float near, float far);
	void lookAt(const glm::vec3& from, const glm::vec3& target, const glm::vec3& up = glm::vec3(0, 1, 0));

	inline const glm::mat4x4& projection() const { return mProjection; }
	inline const glm::mat4x4& world() const { return transform.matrix(); }

private:
	glm::mat4x4 mProjection;
};
