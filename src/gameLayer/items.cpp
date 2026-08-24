#include "items.hpp"

#include "asserts.h"
#include "assetManager.hpp"
#include "constants.hpp"
#include "helpers.hpp"
#include "walls.hpp"

Texture getTextureForItemType(std::uint16_t type, AssetManager& assetManager) {
	if (isBlock(type) || isWall(type)) {
		// block or wall
		return assetManager.textures;
	} else if (isItem(type)) {
		return assetManager.items;
	}

	permaAssertCommentDevelopment(false, "Unknown type");
	return assetManager.textures;
}

Rectangle getTextureCoordsForItemType(std::uint16_t type) {
	if (isBlock(type) || isWall(type)) {
		// block or wall
		return getTextureAtlas(type, 4, Constants::CellSize, Constants::CellSize);
	} else if (isItem(type)) {
		return getTextureAtlas(
		    type - Wall::WALLS_END, 0, Constants::CellSize, Constants::CellSize);
	}

	permaAssertCommentDevelopment(false, "Unknown type");
	return getTextureAtlas(0, 0, Constants::CellSize, Constants::CellSize);
}
