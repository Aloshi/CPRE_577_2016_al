#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <openglpp/Camera.h>

namespace Debug
{

void drawPath(const std::vector<glm::vec3>& pts, glm::vec3 color = glm::vec3(1, 1, 1));
void drawLines(const std::vector<glm::vec3>& pts, glm::vec3 color = glm::vec3(1, 1, 1));

void draw(const Camera& camera);
void clear();

}