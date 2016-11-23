#include <openglpp/Transform.h>

Transform::Transform()
	: mTranslation(0, 0, 0), mScale(1.0f, 1.0f, 1.0f)
{

}

void Transform::rebuildMatrix()
{
	//mMatrix = glm::translate(glm::mat4(1.0f), mTranslation) * glm::mat4_cast(mRotation) * glm::scale(glm::mat4(1.0f), mScale);
	//mMatrix = glm::scale(glm::mat4(1.0f), mScale) * glm::mat4_cast(mRotation) * glm::translate(glm::mat4(1.0f), mTranslation);
	mMatrix = glm::mat4_cast(mRotation) * glm::translate(glm::mat4(1.0f), mTranslation);

	mRight = glm::vec3(1, 0, 0) * mRotation;
	mUp = glm::vec3(0, 1, 0) * mRotation;
	mForward = glm::vec3(0, 0, 1) * mRotation;
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
