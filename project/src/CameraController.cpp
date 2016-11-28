#include "CameraController.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <GLFW/glfw3.h>
#include "Util.h"  // for clamp

CameraController::CameraController(const std::shared_ptr<Camera>& cam)
	: mRotating(false), mRotation(0, 0), mMove(0, 0, 0), mCamera(cam), mRotateSpeed(0.005f), mMoveSpeed(1.0f)
{
}

void CameraController::updateMoveDir(GLFWwindow* window)
{
	mMove.x = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) ? +1 : 0
		+ (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) ? -1 : 0;
	mMove.y = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) ? +1 : 0
		+ (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) ? -1 : 0;
	mMove.z = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) ? +1 : 0
		+ (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? -1 : 0;
}

void CameraController::onMouseButton(GLFWwindow* window, int button, bool pressed, int modifiers)
{
	if (button == 1) {
		mRotating = pressed;
		glfwSetInputMode(window, GLFW_CURSOR, pressed ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}
}

void CameraController::onMouseMoved(float dx, float dy)
{
	if (mRotating) {
		mRotation.x += dx * mRotateSpeed;
		mRotation.y = clamp(mRotation.y + dy * mRotateSpeed, -glm::pi<float>() / 2.0f, glm::pi<float>() / 2.0f);
	}
}

void CameraController::update(float dt)
{
	if (!mCamera)
		return;

	// rotation
	glm::quat rot = glm::angleAxis(mRotation.y, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::angleAxis(mRotation.x, glm::vec3(0.0f, 1.0f, 0.0f));
	mCamera->setRotation(rot);

	// movement
	glm::vec3 pos = mCamera->position();
	pos += mCamera->right() * (float)mMove.x * -mMoveSpeed;
	pos += mCamera->forward() * (float)mMove.y * mMoveSpeed;
	pos += mCamera->up() * (float)mMove.z * -mMoveSpeed;
	mCamera->setPosition(pos);
}
