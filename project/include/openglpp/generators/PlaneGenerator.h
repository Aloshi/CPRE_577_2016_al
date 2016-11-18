#pragma once

#include <vector>
#include <glm/glm.hpp>

void generatePlane(float size, const glm::vec3& normal, const glm::vec3& right, std::vector<glm::vec3>& pts_out, std::vector<glm::vec3>& normals_out);
