#include "gameMap.hpp"

#include "asserts.h"

void GameMap::create(int w, int h) {
	*this = { w, h };
	mapData.resize(w * h);
	wallData.resize(w * h);
}

bool GameMap::inBounds(Vector2 position) {
	return position.x >= 0 && position.y >= 0 && position.x < w && position.y < h;
}

MapCell GameMap::hoveredCell(Vector2 mousePosWorld) {
	int x { static_cast<int>(mousePosWorld.x) };
	int y { static_cast<int>(mousePosWorld.y) };

	return { x, y, blockSafe(x, y), wallSafe(x, y) };
}

Block& GameMap::blockUnsafe(int x, int y) {
	permaAssertCommentDevelopment(
	    mapData.size() == w * h, "Map data not initialized");
	permaAssertCommentDevelopment(
	    x >= 0 && y >= 0 && x < w && y < h, "blockUnsafe out of bounds error");

	return mapData[y * w + x];
}

Block* GameMap::blockSafe(int x, int y) {
	permaAssertCommentDevelopment(
	    mapData.size() == w * h, "Map data not initialized");

	if (x < 0 || y < 0 || x >= w || y >= h) {
		return nullptr;
	}

	return &mapData[y * w + x];
}

Wall& GameMap::wallUnsafe(int x, int y) {

	permaAssertCommentDevelopment(
	    wallData.size() == w * h, "Wall data not initialized");
	permaAssertCommentDevelopment(
	    x >= 0 && y >= 0 && x < w && y < h, "wallUnsafe out of bounds error");

	return wallData[y * w + x];
}

Wall* GameMap::wallSafe(int x, int y) {
	permaAssertCommentDevelopment(
	    wallData.size() == w * h, "Wall data not initialized");

	if (x < 0 || y < 0 || x >= w || y >= h) {
		return nullptr;
	}

	return &wallData[y * w + x];
}
