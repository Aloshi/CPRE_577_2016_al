#pragma once

#include <memory>
#include <string>
#include <openglpp/Object.h>

std::shared_ptr<Object> loadObj(const std::string& path, glm::bvec3 centerAxes = glm::bvec3(false, false, false), const glm::quat& rot = glm::quat());