#pragma once

#include <vector>
#include <string>

std::vector<unsigned char> loadBMP(const std::string& path, unsigned int* width_out, unsigned int* height_out);