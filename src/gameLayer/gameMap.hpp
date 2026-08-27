#pragma once

#include <vector>

#include "blocks.hpp"
#include "raylib.h"
#include "walls.hpp"
#include "worldGenerator.hpp"

struct MapCell {
	int x {};
	int y {};
	Block* block {};
	Wall* wall {};
};

struct GameMap {
	int w {};
	int h {};
	std::vector<Biome> biomes {};

	std::vector<Block> mapData;
	std::vector<Wall> wallData;

	void create(int w, int h);

	bool inBounds(Vector2 position);

	MapCell hoveredCell(Vector2 mousePosWorld);

	Block& blockUnsafe(int x, int y);
	Block* blockSafe(int x, int y);

	Wall& wallUnsafe(int x, int y);
	Wall* wallSafe(int x, int y);

	bool shouldSave {};
};
