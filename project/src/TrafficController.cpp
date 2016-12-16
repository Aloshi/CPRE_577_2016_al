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
	mVelocity = 4.0f;
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

	glm::vec3 pos = c0.pos;
	glm::vec3 forward = glm::normalize(c1.pos);
	glm::mat4 mat = glm::lookAt(glm::vec3(0, 0, 0), forward, c0.normal);
	glm::quat rot = glm::conjugate(glm::toQuat(mat));

	mObject->transform.setPosition(pos);
	mObject->transform.setRotation(rot);
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

	while (mAgents.size() > mTargetPopulation) {
		mAgents.pop_back();
	}
}

void TrafficController::spawnAgent()
{
	auto agent = std::unique_ptr<TrafficAgent>(new TrafficAgent());
	
	const float length = 4.0f;
	const auto& nodes = mGraph.nodes();

	int attempts = 0;
	
	RoadGraph::Node* node = NULL;
	float d = -1.0f;

	// terrible way to avoid spawning inside an existing car
	// a better way would be to make a list of nodes, shuffle it,
	// then grab a node, pick a random initial d, and check up/down the 
	// node until we find a free space; if we don't find one, pick the next
	// node from the shuffled list. if we run out of nodes, there are no free
	// spots remaining
	while (++attempts < 50) {
		node = nodes.at(rand() % nodes.size());
		const float len = node->data.spline.total_length();
		const float min = std::min(length / 2.0f, len);
		const float max = std::max(len - length / 2.0f, 0.0f);

		d = randf(min, max);
		bool ok = true;
		for (unsigned int j = 0; j < mAgents.size(); j++) {
			auto other = mAgents.at(j);
			if (other->node() == node && fabs(d - other->distance()) < length) {
				ok = false;
				break;
			}
		}

		if (ok)
			break;
	}
	agent->start(node, d);

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
