#pragma once

#include <memory>
#include <openglpp/Camera.h>
#include <openglpp/Window.h>

class CameraController {
public:
	CameraController(const std::shared_ptr<Camera>& cam);
	
	void update(float dt);

	inline const std::shared_ptr<Camera>& camera() const { return mCamera; }

	void updateMoveDir(GLFWwindow* window);
	void onMouseButton(GLFWwindow* window, int button, bool pressed, int modifiers);
	void onMouseMoved(float dx, float dy);

private:
	bool mRotating;
	float mMoveSpeed;
	float mRotateSpeed;

	glm::ivec3 mMove;
	glm::vec2 mRotation;
	std::shared_ptr<Camera> mCamera;
};