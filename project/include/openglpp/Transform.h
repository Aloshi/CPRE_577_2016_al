#pragma once

#include <glm/glm.hpp>

#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform {
public:
	Transform();

	inline const glm::vec3& position() const { return mTranslation; }
	void setPosition(const glm::vec3& pos);

	const glm::quat& rotation() const { return mRotation; }
	void setRotation(const glm::quat& rot);

	inline const glm::vec3& right() const { return mRight; }
	inline const glm::vec3& up() const { return mUp; }
	inline const glm::vec3& forward() const { return mForward; }
	inline const glm::mat4x4& matrix() const { return mMatrix; }

	// allow implicit cast to glm::mat4
	inline operator const glm::mat4x4&() const { return mMatrix; }
	inline operator glm::mat4x4&() { return mMatrix; }

private:
	void rebuildMatrix();

	glm::mat4x4 mMatrix;

	// decomposition
	glm::vec3 mTranslation;
	glm::quat mRotation;
	glm::vec3 mScale;

	// cached vectors
	glm::vec3 mRight;
	glm::vec3 mUp;
	glm::vec3 mForward;
};
