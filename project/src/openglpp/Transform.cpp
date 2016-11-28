#include <openglpp/Transform.h>

#include <glm/gtx/transform.hpp>

Transform::Transform()
	: mTranslation(0, 0, 0), mScale(1.0f, 1.0f, 1.0f)
{

}

void Transform::rebuildMatrix()
{
	//mMatrix = glm::translate(glm::mat4(1.0f), mTranslation) * glm::mat4_cast(mRotation) * glm::scale(glm::mat4(1.0f), mScale);
	//mMatrix = glm::scale(glm::mat4(1.0f), mScale) * glm::mat4_cast(mRotation) * glm::translate(glm::mat4(1.0f), mTranslation);
	glm::mat4 trans = glm::translate(mTranslation);
	glm::mat4 rot = glm::mat4(mRotation);
	glm::mat4 scale = glm::scale(mScale);

	mMatrix = trans * rot * scale;
	//mMatrix = glm::translate(mTranslation) * glm::mat4_cast(mRotation) * glm::scale(glm::mat4(1.0f), mScale);

	mRight = glm::vec3(rot * glm::vec4(1, 0, 0, 0));
	mUp = glm::vec3(rot * glm::vec4(0, 1, 0, 0));
	mForward = glm::vec3(rot * glm::vec4(0, 0, 1, 0));

	/*mRight = mRotation * glm::vec3(1, 0, 0);
	mUp = mRotation * glm::vec3(0, 1, 0);
	mForward = mRotation * glm::vec3(0, 0, 1);*/
}

void Transform::setPosition(const glm::vec3& pos)
{
	mTranslation = pos;
	rebuildMatrix();
}

void Transform::setRotation(const glm::quat& rot)
{
	mRotation = rot;
	rebuildMatrix();
}

void Transform::setScale(const glm::vec3& scale)
{
	mScale = scale;
	rebuildMatrix();
}

void Transform::setScale(float scale)
{
	setScale(glm::vec3(scale, scale, scale));
}