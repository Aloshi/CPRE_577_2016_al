#pragma once

#include <vector>
#include <glm/glm.hpp>

void generateSphere(int n, float radius, std::vector<glm::vec3>& pts_out,
	std::vector<glm::vec3>& normals_out);