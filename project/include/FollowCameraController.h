#pragma once

#include <memory>
#include "CameraController.h"
#include <openglpp/Object.h>

class TrafficController;
class TrafficAgent;

class FollowCameraController : public CameraController {
public:
	FollowCameraController(const std::shared_ptr<Camera>& cam, const TrafficController* tc);
	
	virtual void update(float dt) override;
private:
	void pickNewTarget();

	const TrafficController* mTC;
	std::shared_ptr<TrafficAgent> mTargetAgent;
	std::shared_ptr<Object> mTargetObject;
};
