#pragma once

#include <memory>
#include <string>
#include <openglpp/Object.h>

std::vector< std::shared_ptr<Object> > loadObj(const std::string& path);