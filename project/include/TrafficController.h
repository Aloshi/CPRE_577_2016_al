#pragma once

#include <RoadGraph.h>

class TrafficController;

class TrafficAgent
{
public:
	TrafficAgent();

	void start(const RoadGraph::Node* path, float d);
	void update(const TrafficController& tc, float dt);

	void render();

	enum State {
		DRIVING,
		WAITING,
		DEAD_END
	};

	inline State state() const { return mState; }

private:
	static const int N_TYPES;
	static std::vector< std::shared_ptr<Object> > sTemplates;
	static std::shared_ptr<Object> createObject(int type);

	int mType;
	float mPosition;
	float mVelocity;
	const RoadGraph::Node* mCurrentNode;
	State mState;

	std::shared_ptr<Object> mObject;  // guaranteed to exist
};

class TrafficController
{
public:
	void setGraph(const RoadGraph& graph);

	void setTargetPopulation(unsigned int count);
	void spawnAgent();

	void update(float dt);
	void render();

private:
	RoadGraph mGraph;

	unsigned int mTargetPopulation;
	std::vector< std::unique_ptr<TrafficAgent> > mAgents;
};