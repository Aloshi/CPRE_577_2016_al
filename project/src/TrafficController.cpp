#include "TrafficController.h"
#include <openglpp/LoadObj.h>
#include "Util.h"  // for randf
#include <algorithm>  // min/max
#include <openglpp/Debug.h>
#include <iostream>

#include <glm/gtx/quaternion.hpp>  // for toQuat
#include <glm/gtx/transform.hpp>

const int TrafficAgent::N_TYPES = 1;
std::vector< std::shared_ptr<Object> > TrafficAgent::sTemplates;

std::shared_ptr<Object> TrafficAgent::createObject(int type)
{
	static bool sInitialized = false;
	if (!sInitialized) {
		sTemplates.reserve(N_TYPES);

		{
			glm::bvec3 center(false, false, false);
			glm::quat rot = glm::toQuat(glm::rotate(glm::pi<float>() / 2.0f * 3, glm::vec3(0, 1, 0)));
			sTemplates.push_back(loadObj("../models/kia_rio/kia_rio.obj", center, rot));
			sTemplates.back()->transform.setScale(glm::vec3(0.3f, 0.3f, 0.3f));
		}

		sInitialized = true;
		assert(N_TYPES == sTemplates.size());
	}

	assert(type >= 0 && (unsigned int)type < sTemplates.size());

	return sTemplates.at(type)->clone();
}

TrafficAgent::TrafficAgent()
	: mPosition(0.0f), mVelocity(0.0f), mCurrentNode(NULL), mState(WAITING)
{
	mType = rand() % N_TYPES;
	mObject = createObject(mType);
}

void TrafficAgent::start(const RoadGraph::Node* path, float d)
{
	mCurrentNode = path;
	mPosition = clamp(d, 0.0f, path->data.spline.total_length());
	mVelocity = 3.0f;
	mState = DRIVING;
}

void TrafficAgent::update(const TrafficController& tc, float dt)
{
	if (mCurrentNode == NULL || mState == DEAD_END)
		return;

	if (mState == WAITING)
		return;

	mPosition += dt * mVelocity;
	
	const float splineLength = mCurrentNode->data.spline.total_length();
	if (mPosition > splineLength) {
		// at the end of our spline!
		if (mCurrentNode->edges.size() == 0) {
			mState = DEAD_END;
			mPosition = splineLength;
		} else {
			// pick a place to go now
			const auto& edges = mCurrentNode->edges;
			RoadGraph::Node* nextNode = edges.at(rand() % edges.size()).to;
			mCurrentNode = nextNode;
			mPosition -= splineLength;
		}
	}

	// update object position/rotation
	RoadVertex c0, c1;
	mCurrentNode->data.spline.evaluate(mPosition, &c0, &c1);

	//Debug::drawLines({ c0.pos, c0.pos + c0.normal });

	glm::vec3 pos = c0.pos;
	glm::vec3 forward = glm::normalize(c1.pos);
	glm::mat4 mat = glm::lookAt(glm::vec3(0, 0, 0), forward, c0.normal);
	glm::quat rot = glm::conjugate(glm::toQuat(mat));

	mObject->transform.setPosition(pos);
	mObject->transform.setRotation(rot);

	/*static float totalt = 0.0f;
	totalt += dt;
	mObject->transform.setRotation(glm::angleAxis(sinf(totalt)* glm::pi<float>(), glm::vec3(0, 1, 0)));*/
}

void TrafficAgent::render()
{
	mObject->render();
}


void TrafficController::setGraph(const RoadGraph& graph)
{
	mAgents.clear();
	mGraph = graph;
}

void TrafficController::setTargetPopulation(unsigned int count)
{
	mTargetPopulation = count;
}

void TrafficController::spawnAgent()
{
	auto agent = std::make_unique<TrafficAgent>();
	
	const auto& nodes = mGraph.nodes();
	const RoadGraph::Node* node = nodes.at(rand() % nodes.size());
	
	const float len = node->data.spline.total_length();
	float min = std::min(2.0f, len);
	float max = std::max(len - 2.0f, 0.0f);
	agent->start(node, randf(min, max));

	mAgents.push_back(std::move(agent));
}

void TrafficController::update(float dt)
{
	// spawn new things
	for (unsigned int i = mAgents.size(); i < mTargetPopulation; i++) {
		spawnAgent();
	}

	for (unsigned int i = 0; i < mAgents.size(); i++) {
		mAgents.at(i)->update(*this, dt);
	}

	// remove dead things
	{
		auto it = mAgents.begin();
		while (it != mAgents.end()) {
			if (it->get()->state() == TrafficAgent::DEAD_END) {
				it = mAgents.erase(it);
				std::cout << "Removing dead agent\n";
			} else {
				it++;
			}
		}
	}
}

void TrafficController::render()
{
	for (unsigned int i = 0; i < mAgents.size(); i++) {
		mAgents.at(i)->render();
	}
}