#pragma once

#include <vector>

#include "blocks.hpp"
#include "gameMap.hpp"
#include "raylib.h"
#include "walls.hpp"

struct Structure {
	int w {};
	int h {};

	std::vector<Block> mapData;
	std::vector<Wall> wallData;

	void create(int w, int h);

	Block& blockUnsafe(int x, int y);
	Block* blockSafe(int x, int y);

	Wall& wallUnsafe(int x, int y);
	Wall* wallSafe(int x, int y);

	void copyFromMap(GameMap& map, Vector2 start, Vector2 end);
	void pasteIntoMap(GameMap& map, Vector2 start);
};
