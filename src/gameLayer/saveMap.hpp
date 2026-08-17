#pragma once

#include <vector>

#include "blocks.hpp"
#include "walls.hpp"

bool saveMapDataToFile(const std::vector<Block> &blocks,
                       const std::vector<Wall> &walls, int w, int h,
                       const char *fileName);

bool loadMapDataFromFile(std::vector<Block> &blocks, std::vector<Wall> &walls,
                         int &w, int &h, const char *fileName);
