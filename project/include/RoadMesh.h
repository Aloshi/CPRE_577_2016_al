#pragma once

#include "Spline.h"
#include <openglpp/Mesh.h>
#include <openglpp/Object.h>
#include <glm/vec3.hpp>
#include <memory>

// splineable road vertex
struct RoadVertex {
	glm::vec3 pos;
	glm::vec3 normal;

	inline RoadVertex operator+(const RoadVertex& rhs) const {
		return RoadVertex{ pos + rhs.pos, normal + rhs.normal };
	}
	inline RoadVertex operator-(const RoadVertex& rhs) const {
		return RoadVertex{ pos - rhs.pos, normal - rhs.normal };
	}
	inline float len() const {
		return glm::length(pos);
	}
};

inline RoadVertex operator*(float lhs, const RoadVertex& rhs) {
	return RoadVertex{ rhs.pos * lhs, rhs.normal * lhs };
}



struct Road;
struct Intersection;
struct IntersectionEdge;

// connects a road and intersection
// can never do road-road or intersection-intersection connections
struct RoadConnection
{
	std::weak_ptr<Road> road;
	int roadEnd;

	std::weak_ptr<Intersection> intersection;
	int intersectionVertIdx;

	inline bool valid() const { return !intersection.expired() && !road.expired(); }
	const RoadVertex& roadVertex() const;
	const IntersectionEdge& intersectionEdge() const;
};

struct Road
{
	std::vector<RoadVertex> vertices;
	std::vector< std::shared_ptr<RoadConnection> > connections;

	CatmullRom<RoadVertex> generateSpline(int lane) const;
	std::shared_ptr<Object> generateObject() const;  // sets up textures/materials

	inline int lanes() const { return 2; }

private:
	std::shared_ptr<Mesh> generateMesh() const;

	inline float centerPadding() const { return 0.0f; }
	inline float lanePadding() const { return 0.0f; }
	inline float laneWidth() const { return 3.0f; }
	inline float gutterWidth() const { return 0.0f; }
	inline float height() const { return 1.0f; }
	inline float totalWidth() const {
		return centerPadding() * 2 + laneWidth() * lanes() + lanePadding() * (lanes() - 2) + gutterWidth() * 2;
	}

};

// an entrance/exit to an intersection
struct IntersectionEdge {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 forward;

	inline int lanes() const { return 2; }
	inline float centerPadding() const { return 0.0f; }
	inline float lanePadding() const { return 0.0f; }
	inline float laneWidth() const { return 3.0f; }
};

// to be honest, edges should be a constant in model space, and with an
// extra transform on the Intersection object itself; but I don't have
// time to debug that right now
struct Intersection
{
	std::vector<IntersectionEdge> edges;
	std::vector< std::shared_ptr<RoadConnection> > connections;

	CatmullRom<RoadVertex> generateSpline(int fromVtx, int toVtx, int lane) const;
	std::shared_ptr<Object> generateObject() const;
};

void connect(const std::shared_ptr<Road>& road, int roadEnd, const std::shared_ptr<Intersection>& intersection, int vtxId);