#pragma once

#include "Spline.h"
#include <openglpp/Mesh.h>
#include <openglpp/Object.h>
#include <glm/vec3.hpp>
#include <memory>

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
		return (float)pos.length();
	}
};

inline RoadVertex operator*(float lhs, const RoadVertex& rhs) {
	return RoadVertex{ rhs.pos * lhs, rhs.normal * lhs };
}



struct Road;
struct Intersection;



struct RoadConnection
{
	std::weak_ptr<Intersection> intersection;
	int vertIdx;

	inline bool valid() const { return !intersection.expired(); }
	const RoadVertex& vertex() const;
};

struct IntersectionConnection {
	std::weak_ptr<Road> road;
	int end;

	inline bool valid() const { return !road.expired(); }
	const RoadVertex& vertex() const;
};



struct Road
{
	std::vector<RoadVertex> vertices;
	RoadConnection connections[2];

	CatmullRom<RoadVertex> generateSpline(int lane) const;
	std::shared_ptr<Mesh> generateMesh() const;
	std::shared_ptr<Object> generateObject() const;  // sets up textures/materials

private:
	inline float laneWidth() const { return 3.0f; }
	inline float height() const { return 1.0f; }
	inline int lanes() const { return 2; }
	inline float totalWidth() const { return lanes() * laneWidth(); }

};

struct Intersection
{
	std::vector<RoadVertex> vertices;
	std::vector<IntersectionConnection> connections;

	CatmullRom<RoadVertex> generateSpline(int from, int to) const;
	// std::shared_ptr<Mesh> generateMesh() const;
};