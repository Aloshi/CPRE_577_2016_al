#include "RoadMesh.h"
#include "Util.h"

#include <openglpp/LoadObj.h>

#include <iostream>

const RoadVertex& RoadConnection::roadVertex() const {
	std::shared_ptr<Road> road = this->road.lock();
	return roadEnd == 0 ? road->vertices.front() : road->vertices.back();
}

const IntersectionEdge& RoadConnection::intersectionEdge() const {
	return intersection.lock()->edges.at(intersectionVertIdx);
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

CatmullRom<RoadVertex> Road::generateSpline(int lane) const
{
	CatmullRom<RoadVertex> spline;
	std::vector<RoadVertex> splineVerts = this->vertices;
	spline.set_control_points(splineVerts, true);
	if (lane == -1)
		return spline;

	auto slices = buildSlices(spline, totalWidth(), height());

	const int laneThisSide = lane / 2;
	const float padding = centerPadding() + laneThisSide * (lanePadding() + laneWidth());
	const int dir = lane % 2 == 0 ? 1 : -1;

	float t = 0.5f + ((padding + laneWidth() / 2.0f) / totalWidth()) * dir;
	splineVerts.resize(slices.size());
	for (unsigned int i = 0; i < slices.size(); i++) {
		glm::vec3 vec = (slices[i].verts[TOP_RIGHT] - slices[i].verts[TOP_LEFT]);
		glm::vec3 p = slices[i].verts[TOP_LEFT] + vec * t;
		glm::vec3 n = glm::normalize(slices[i].verts[TOP_LEFT] - slices[i].verts[BOTTOM_LEFT]);
		splineVerts[i].pos = p;
		splineVerts[i].normal = n;
	}

	// flip if it's an odd lane
	if (lane % 2 != 0)
		std::reverse(splineVerts.begin(), splineVerts.end());

	spline.set_control_points(splineVerts, true);
	return spline;
}

std::shared_ptr<Mesh> Road::generateMesh() const
{
	// need points, normals, and indices
	std::vector<unsigned int> indices;
	std::vector<RoadSlice> slices = buildSlices(generateSpline(-1), totalWidth(), height());

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
		const glm::vec3 normal = -glm::normalize(glm::cross(points[i + 1] - points[i], points[i + 2] - points[i]));
		for (unsigned int j = 0; j < 3; j++) {
			normals[i + j] = normal;
		}
	}

	// texture top of road
	std::vector<glm::vec2> texCoords(indices.size());
	const float texScale = 0.25f;
	const unsigned int topIdxStart = 12;
	float totalLeftDist = 0.0f;
	float totalRightDist = 0.0f;
	for (unsigned int i = 0; i < slices.size() - 1; i++) {
		unsigned int base = topIdxStart + i * 24;
		float leftDist = glm::length(points[base + 0] - points[base + 2]) * texScale;
		float rightDist = glm::length(points[base + 4] - points[base + 5]) * texScale;
		const float avg = (leftDist + rightDist) / 2.0f;
		leftDist = rightDist = avg;

		// 0/1 should be based on totalWidth()
		texCoords[base + 0] = glm::vec2(0, totalLeftDist + leftDist);
		texCoords[base + 1] = glm::vec2(1, totalRightDist);
		texCoords[base + 2] = glm::vec2(0, totalLeftDist);

		texCoords[base + 3] = glm::vec2(0, totalLeftDist + leftDist);
		texCoords[base + 4] = glm::vec2(1, totalRightDist + rightDist);
		texCoords[base + 5] = glm::vec2(1, totalRightDist);

		totalLeftDist += leftDist;
		totalRightDist += rightDist;
	}

	auto mesh = std::make_shared<Mesh>();
	mesh->setVertices(points, GL_TRIANGLES);
	mesh->setNormals(normals);
	mesh->setTexCoords(texCoords);
	return mesh;
}

static bool sInitializedTextures = false;
static std::shared_ptr<Texture> sTextures[3];

std::shared_ptr<Object> Road::generateObject() const
{
	auto obj = std::make_shared<Object>();
	obj->setMesh(generateMesh());

	if (!sInitializedTextures) {
		sTextures[0] = Texture::fromFile("../textures/Seamless_Asphalt_Texture.bmp");
		sTextures[1] = Texture::fromFile("../textures/Seamless_Asphalt_Texture_NORMAL.bmp");
		sTextures[2] = Texture::fromFile("../textures/Seamless_Asphalt_Texture_SPECULAR.bmp");

		for (int i = 0; i < 3; i++)
			sTextures[i]->setWrapMode(GL_REPEAT, GL_REPEAT);

		sInitializedTextures = true;
	}

	obj->material.setTexture(0, sTextures[0]);
	obj->material.setTexture(1, sTextures[1]);
	obj->material.setTexture(2, sTextures[2]);

	obj->material.set(Shader::MAT_DIFFUSE_TYPE, Shader::SOURCE_TEXTURE);
	obj->material.set(Shader::MAT_DIFFUSE_TEXTURE, 0);

	obj->material.set(Shader::MAT_USE_NORMAL_MAP, 1);
	obj->material.set(Shader::MAT_NORMAL_TEXTURE, 1);

	obj->material.set(Shader::MAT_SPECULAR_TYPE, Shader::SOURCE_TEXTURE);
	obj->material.set(Shader::MAT_SPECULAR_TEXTURE, 2);

	obj->material.set(Shader::MAT_SHININESS, 6.0f);

	return obj;
}

RoadVertex edgeToVertex(const IntersectionEdge& e, int lane)
{
	int laneThisSide = lane / 2;
	float dir = (lane % 2 == 0) ? 1 : -1;

	glm::vec3 right = glm::normalize(glm::cross(e.forward, e.normal));

	float padding = (e.lanePadding() + e.laneWidth()) * laneThisSide;
	glm::vec3 pos = e.pos + right * (e.centerPadding() + padding + e.laneWidth() / 2.0f) * dir;

	return RoadVertex{ pos, e.normal };
}

CatmullRom<RoadVertex> Intersection::generateSpline(int fromVtx, int toVtx, int lane) const
{
	CatmullRom<RoadVertex> spline;

	const auto& fromEdge = edges.at(fromVtx);
	const auto& toEdge = edges.at(toVtx);

	int otherLane = (lane % 2 == 0) ? lane + 1 : lane - 1;

	std::vector<RoadVertex> pts;
	pts.push_back(edgeToVertex(fromEdge, lane));
	pts.push_back(edgeToVertex(toEdge, otherLane));

	spline.set_control_points(pts, true);

	return spline;
}

std::shared_ptr<Object> Intersection::generateObject() const
{
	auto obj = loadObj("../models/intersection_4way.obj");
	glm::vec3 center(0, 0, 0);
	for (unsigned int i = 0; i < edges.size(); i++) {
		center += edges[i].pos;
	}
	center /= edges.size();
	obj->transform.setPosition(center);
	return obj;
}

void connect(const std::shared_ptr<Road>& road, int roadEnd, const std::shared_ptr<Intersection>& intersection, int vtxId)
{
	assert(roadEnd == 0 || roadEnd == 1);
	assert(vtxId < (int)intersection->edges.size() && vtxId >= 0);
	assert(road != nullptr && intersection != nullptr);

	auto conn = std::make_shared<RoadConnection>();
	conn->road = road;
	conn->roadEnd = roadEnd;
	conn->intersection = intersection;
	conn->intersectionVertIdx = vtxId;

	road->connections.push_back(conn);
	intersection->connections.push_back(conn);
}