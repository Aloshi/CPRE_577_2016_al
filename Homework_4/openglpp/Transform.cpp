#include "Transform.h"

Transform::Transform()
	: mTranslation(0, 0, 0), mScale(1.0f, 1.0f, 1.0f)
{

}

void Transform::rebuildMatrix()
{
	mMatrix = glm::translate(glm::mat4(1.0f), mTranslation) * glm::mat4_cast(mRotation) * glm::scale(glm::mat4(1.0f), mScale);
	//mMatrix = glm::scale(glm::mat4(1.0f), mScale) * glm::mat4_cast(mRotation) * glm::translate(glm::mat4(1.0f), mTranslation);
	mRight = mRotation * glm::vec3(1, 0, 0);
	mUp = mRotation * glm::vec3(0, 1, 0);
	mForward = mRotation * glm::vec3(0, 0, 1);
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
