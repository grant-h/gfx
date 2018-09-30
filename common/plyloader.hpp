#ifndef PLYLOADER_HPP
#define PLYLOADER_HPP

#include <vector>

#include "Object.h"

bool load_ply(const char * file, std::vector<Vertex> & data, std::vector<unsigned int> & mindices);

#endif
