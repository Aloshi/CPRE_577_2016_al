#pragma once

#include <memory>
#include <openglpp/Mesh.h>

#include <vector>
#include <glm/glm.hpp>

std::shared_ptr<Mesh> generatePlane(float size, const glm::vec3& normal, const glm::vec3& right);
