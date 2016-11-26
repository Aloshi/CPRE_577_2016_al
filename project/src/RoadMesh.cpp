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