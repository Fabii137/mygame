
#include <utility>

#include "structure.hpp"

#include "asserts.h"

void Structure::create(int w, int h) {
	*this = { w, h };
	mapData.resize(w * h);
	wallData.resize(w * h);
}

Block& Structure::blockUnsafe(int x, int y) {
	permaAssertCommentDevelopment(
	    mapData.size() == w * h, "Map data not initialized");
	permaAssertCommentDevelopment(
	    x >= 0 && y >= 0 && x < w && y < h, "getBlockUnsafe out of bounds error");

	return mapData[y * w + x];
}

Block* Structure::blockSafe(int x, int y) {
	permaAssertCommentDevelopment(
	    mapData.size() == w * h, "Map data not initialized");

	if (x < 0 || y < 0 || x >= w || y >= h) {
		return nullptr;
	}

	return &mapData[y * w + x];
}

Wall& Structure::wallUnsafe(int x, int y) {

	permaAssertCommentDevelopment(
	    wallData.size() == w * h, "Wall data not initialized");
	permaAssertCommentDevelopment(
	    x >= 0 && y >= 0 && x < w && y < h, "getWallUnsafe out of bounds error");

	return wallData[y * w + x];
}

Wall* Structure::wallSafe(int x, int y) {
	permaAssertCommentDevelopment(
	    wallData.size() == w * h, "Wall data not initialized");

	if (x < 0 || y < 0 || x >= w || y >= h) {
		return nullptr;
	}

	return &wallData[y * w + x];
}

void Structure::copyFromMap(GameMap& map, Vector2 start, Vector2 end) {
	if (end.x > map.w) {
		end.x = map.w - 1;
	}
	if (start.x > map.w) {
		start.x = map.w - 1;
	}

	if (end.y > map.h) {
		end.y = map.h - 1;
	}
	if (start.y > map.h) {
		start.y = map.h - 1;
	}

	if (end.x < 0) {
		end.x = 0;
	}
	if (end.y < 0) {
		end.y = 0;
	}
	if (start.x < 0) {
		start.x = 0;
	}
	if (start.y < 0) {
		start.y = 0;
	}

	if (start.x > end.x) {
		std::swap(start.x, end.x);
	}
	if (start.y > end.y) {
		std::swap(start.y, end.y);
	}

	Vector2 size { end.x - start.x + 1, end.y - start.y + 1 };
	if (size.x > map.w || size.y > map.h) {
		return;
	}

	create(size.x, size.y);

	for (int y {}; y < size.y; y++) {
		for (int x {}; x < size.x; x++) {
			blockUnsafe(x, y) = map.blockUnsafe(x + start.x, y + start.y);
			wallUnsafe(x, y) = map.wallUnsafe(x + start.x, y + start.y);
		}
	}
};

void Structure::pasteIntoMap(GameMap& map, Vector2 start) {
	for (int y {}; y < h; y++) {
		for (int x {}; x < w; x++) {
			Block* block { map.blockSafe(x + start.x, y + start.y) };
			Wall* wall { map.wallSafe(x + start.x, y + start.y) };

			if (block) {
				*block = blockUnsafe(x, y);
				*wall = wallUnsafe(x, y);
			}
		}
	}
}
