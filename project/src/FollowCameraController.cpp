#include "FollowCameraController.h"
#include "TrafficController.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <GLFW/glfw3.h>
#include "Util.h"  // for clamp

FollowCameraController::FollowCameraController(const std::shared_ptr<Camera>& cam, const TrafficController* tc)
	: CameraController(cam), mTC(tc)
{
}

void FollowCameraController::pickNewTarget()
{
	if (mTC == NULL)
		return;

	mTargetAgent.reset();
	mTargetObject.reset();

	if (mTC->mAgents.size() == 0)
		return;

	mTargetAgent = mTC->mAgents.at(rand() % mTC->mAgents.size());
	mTargetObject = mTargetAgent->mObject;
}

void FollowCameraController::update(float dt)
{
	if (!mCamera)
		return;

	if (!mTargetAgent || mTargetAgent->state() == TrafficAgent::DEAD_END || !mTargetObject)
		pickNewTarget();

	if (!mTargetObject)
		return;

	float followDistY = 3.5f;
	float followDistZ = 7.0f;
	glm::vec3 center = mTargetObject->transform.position()
		         + mTargetObject->transform.rotation() * glm::vec3(0, followDistY, 0);
	glm::vec3 eye = center
		      + mTargetObject->transform.rotation() * glm::angleAxis(mRotation.x, glm::vec3(0, 1, 0)) * glm::vec3(0, 0, followDistZ);
	mCamera->lookAt(eye, center + mTargetObject->transform.rotation() * glm::vec3(0, 0.0f, 0), glm::vec3(0, 1, 0));
}
