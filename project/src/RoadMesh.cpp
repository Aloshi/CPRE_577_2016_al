#include "RoadMesh.h"
#include "Util.h"

#include <iostream>

const RoadVertex& RoadConnection::vertex() const {
	return intersection.lock()->vertices.at(vertIdx);
}

const RoadVertex& IntersectionConnection::vertex() const {
	std::shared_ptr<Road> ptr = road.lock();
	return end == 0 ? ptr->vertices.front() : ptr->vertices.back();
}

CatmullRom<RoadVertex> Road::generateSpline(int lane) const
{
	CatmullRom<RoadVertex> spline;
	std::vector<RoadVertex> splineVerts = this->vertices;
	spline.set_control_points(splineVerts, true);
	return spline;

	// TODO use connections to generate ends
	/*if (connections[0].valid())
		splineVerts.insert(splineVerts.begin(), connections[0].vertex());
	else
		splineVerts.insert(splineVerts.begin(), splineVerts.front());

	if (connections[1].valid())
		splineVerts.insert(splineVerts.end(), connections[1].vertex());
	else
		splineVerts.insert(splineVerts.end(), splineVerts.back());*/

	/*if (lane == -1) {

	} else {
		int dir = (lane % 2 == 0) ? 1 : -1;
		float off = lane / 2 * dir * laneWidth();

		// TODO don't build time map for temporary spline
		// TODO use connections to generate endpoints
		std::vector<RoadVertex> shiftedVerts(splineVerts.size());
		for (unsigned int seg = 1; seg < splineVerts.size() - 2; seg++) {
			RoadVertex c0, c1;
			spline.evaluate_segment(seg, 0.0f, &c0, &c1);
		}
	}*/
}

#define TOP_LEFT 0
#define TOP_RIGHT 1
#define BOTTOM_LEFT 2
#define BOTTOM_RIGHT 3

struct RoadSlice {
	glm::vec3 verts[4];

	RoadSlice() {}
	RoadSlice(glm::vec3 topCenter, glm::vec3 forward, glm::vec3 up, float width, float height) {
		glm::vec3 right = glm::cross(forward, up);
		verts[TOP_LEFT] = topCenter - right * width / 2.0f;
		verts[TOP_RIGHT] = topCenter + right * width / 2.0f;
		verts[BOTTOM_LEFT] = verts[TOP_LEFT] - up * height;
		verts[BOTTOM_RIGHT] = verts[TOP_RIGHT] - up * height;
	}
};

std::vector<RoadSlice> buildSlices(const CatmullRom<RoadVertex>& spline, float width, float height)
{
	const float MIN_METERS_PER_SLICE = 0.1f;
	const float BREAK_ANGLE = glm::radians(3.0f);

	const float totalLength = spline.total_length();
	const int nSlices = (int)std::ceilf(totalLength / MIN_METERS_PER_SLICE);
	const float lengthStep = totalLength / nSlices;

	std::vector<RoadSlice> slices;

	glm::vec3 lastForward;

	// always do start
	{
		RoadVertex start, startDeriv;
		spline.evaluate(0, &start, &startDeriv);
		lastForward = glm::normalize(startDeriv.pos);
		slices.push_back(RoadSlice(start.pos, lastForward, start.normal, width, height));
	}

	/*for (unsigned int i = 0; i < spline.n_time_points(); i++) {
		auto t = spline.time_point(i);
		std::cout << "t[" << i << "]: d=" << t.distance << ", seg=" << t.segment_idx << ", u=" << t.u << "\n";
	}*/
	std::cout << "total length: " << spline.total_length() << "\n";

	for (int i = 1; i < nSlices; i++) {
		const float d = i * lengthStep;

		RoadVertex c0, c1;
		spline.evaluate(d, &c0, &c1);
		const glm::vec3 forward = glm::normalize(c1.pos);
		const float dot = glm::dot(forward, lastForward);
		const float angle = acos(clamp(dot, -1.0f, 1.0f));
		if (angle > BREAK_ANGLE) {
			//std::cout << forward.x << ", " << forward.y << ", " << forward.z << "\n";
			//std::cout << c0.normal.x << ", " << c0.normal.y << ", " << c0.normal.z << "\n";
			lastForward = forward;
			slices.push_back(RoadSlice(c0.pos, forward, c0.normal, width, height));
			std::cout << "adding slice at d=" << d << "\n";
		} else {
			//std::cout << "  SKIP d=" << d << " (angle = " << glm::degrees(angle) << ", forward = " << forward.x << ", " << forward.y << ", " << forward.z << ")\n";
			//std::cout << "skip (angle = " << angle << ")\n";
		}
	}

	// always do end
	{
		RoadVertex end, endDeriv;
		spline.evaluate(spline.total_length(), &end, &endDeriv);
		glm::vec3 forward = glm::normalize(endDeriv.pos);
		slices.push_back(RoadSlice(end.pos, forward, end.normal, width, height));
	}

	return slices;
}

std::shared_ptr<Mesh> Road::generateMesh() const
{
	// need points, normals, and indices
	std::vector<unsigned int> indices;

	CatmullRom<RoadVertex> spline(vertices, true);  // TODO take connections into account for endpoints
	std::vector<RoadSlice> slices = buildSlices(spline, totalWidth(), height());

	indices.resize((slices.size() - 1) * 8 * 3 + 4 * 3);

	unsigned int idx = 0;

	// front
	indices[idx++] = TOP_LEFT;
	indices[idx++] = BOTTOM_RIGHT;
	indices[idx++] = BOTTOM_LEFT;

	indices[idx++] = TOP_LEFT;
	indices[idx++] = TOP_RIGHT;
	indices[idx++] = BOTTOM_RIGHT;

	// sides/top/bottom
	for (unsigned int i = 0; i < slices.size() - 1; i++)
	{
		// left
		indices[idx++] = (i + 1) * 4 + TOP_LEFT;
		indices[idx++] = i * 4 + TOP_LEFT;
		indices[idx++] = i * 4 + BOTTOM_LEFT;

		indices[idx++] = (i + 1) * 4 + TOP_LEFT;
		indices[idx++] = i * 4 + BOTTOM_LEFT;
		indices[idx++] = (i + 1) * 4 + BOTTOM_LEFT;

		// top
		indices[idx++] = (i + 1) * 4 + TOP_LEFT;
		indices[idx++] = i * 4 + TOP_RIGHT;
		indices[idx++] = i * 4 + TOP_LEFT;

		indices[idx++] = (i + 1) * 4 + TOP_LEFT;
		indices[idx++] = (i + 1) * 4 + TOP_RIGHT;
		indices[idx++] = i * 4 + TOP_RIGHT;

		// right
		indices[idx++] = i * 4 + TOP_RIGHT;
		indices[idx++] = (i + 1) * 4 + TOP_RIGHT;
		indices[idx++] = i * 4 + BOTTOM_RIGHT;

		indices[idx++] = i * 4 + BOTTOM_RIGHT;
		indices[idx++] = (i + 1) * 4 + TOP_RIGHT;
		indices[idx++] = (i + 1) * 4 + BOTTOM_RIGHT;

		// bottom
		indices[idx++] = i * 4 + BOTTOM_RIGHT;
		indices[idx++] = (i + 1) * 4 + BOTTOM_LEFT;
		indices[idx++] = i * 4 + BOTTOM_LEFT;

		indices[idx++] = (i + 1) * 4 + BOTTOM_RIGHT;
		indices[idx++] = (i + 1) * 4 + BOTTOM_LEFT;
		indices[idx++] = i * 4 + BOTTOM_RIGHT;
	}

	// back
	indices[idx++] = (slices.size() - 1) * 4 + BOTTOM_RIGHT;
	indices[idx++] = (slices.size() - 1) * 4 + TOP_LEFT;
	indices[idx++] = (slices.size() - 1) * 4 + BOTTOM_LEFT;

	indices[idx++] = (slices.size() - 1) * 4 + TOP_RIGHT;
	indices[idx++] = (slices.size() - 1) * 4 + TOP_LEFT;
	indices[idx++] = (slices.size() - 1) * 4 + BOTTOM_RIGHT;
	assert(idx == indices.size());

	std::vector<glm::vec3> points(indices.size());
	for (unsigned int i = 0; i < indices.size(); i++) {
		unsigned int slice = indices[i] / 4;
		unsigned int vertIdx = indices[i] % 4;
		points[i] = slices[slice].verts[vertIdx];
	}

	std::vector<glm::vec3> normals(indices.size());
	for (unsigned int i = 0; i < indices.size(); i += 3) {
		const glm::vec3 normal = -glm::normalize(glm::cross(points[i+1] - points[i], points[i + 2] - points[i]));
		for (unsigned int j = 0; j < 3; j++) {
			normals[i + j] = normal;
		}
	}

	auto mesh = std::make_shared<Mesh>();
	mesh->setVertices(points, GL_TRIANGLES);
	mesh->setNormals(normals);
	return mesh;
}