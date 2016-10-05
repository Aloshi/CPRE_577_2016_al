#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera {
public:
	Camera(float fov, float aspect, float near = 0.01f, float far = 100.0f);

	void setPerspective(float fovY, float aspect, float near, float far);

	inline const glm::vec3& position() const { return mTranslation; }
	void setPosition(const glm::vec3& pos);

	const glm::quat& rotation() const { return mRotation; }
	void setRotation(const glm::quat& rot);

	void lookAt(const glm::vec3& from, const glm::vec3& target, const glm::vec3& up = glm::vec3(0, 1, 0));

	// accessors
	inline const glm::vec3& right() const { return mRight; }
	inline const glm::vec3& up() const { return mUp; }
	inline const glm::vec3& forward() const { return mForward; }
	inline glm::mat4x4& matrix() { return mMatrix; }

	inline const glm::mat4x4& projection() const { return mProjection; }
	inline const glm::mat4x4& world() const { return mMatrix; }

private:
	void rebuildMatrix();

	glm::mat4x4 mMatrix;

	// decomposition
	glm::mat4x4 mProjection;
	glm::vec3 mTranslation;
	glm::quat mRotation;
	glm::vec3 mScale;

	// cached vectors
	glm::vec3 mRight;
	glm::vec3 mUp;
	glm::vec3 mForward;
};