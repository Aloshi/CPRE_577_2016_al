#pragma once

#include <memory>
#include <openglpp/Mesh.h>

#include <vector>
#include <glm/glm.hpp>

std::shared_ptr<Mesh> generateSphere(int n, float radius);