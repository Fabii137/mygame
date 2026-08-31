#include "items.hpp"

#include "asserts.h"
#include "assetManager.hpp"
#include "constants.hpp"
#include "helpers.hpp"
#include "walls.hpp"

#include "nlohmann/json.hpp"

Texture getTextureForItemType(
    std::uint16_t type, const AssetManager& assetManager) {
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
		return getTextureAtlas(type, 4, Constants::CELL_SIZE, Constants::CELL_SIZE);
	} else if (isItem(type)) {
		return getTextureAtlas(
		    type - Wall::WALLS_END, 0, Constants::CELL_SIZE, Constants::CELL_SIZE);
	}

	permaAssertCommentDevelopment(false, "Unknown type");
	return getTextureAtlas(0, 0, Constants::CELL_SIZE, Constants::CELL_SIZE);
}

Json Item::formatToJson() const {
	Json json {};

	json["type"] = static_cast<std::uint16_t>(type);
	json["count"] = count;

	return json;
}

bool Item::loadFromJson(Json& json) {
	if (!json.is_object()) {
		return false;
	}

	if (!json.contains("type") || !json["type"].is_number_unsigned()) {
		return false;
	}

	if (!json.contains("count") || !json["count"].is_number_unsigned()) {
		return false;
	}

	std::uint16_t rawType = json["type"];
	type = static_cast<Type>(rawType);

	size_t count = json["count"];
	if (count > ::maxStackSize(type)) {
		count = ::maxStackSize(type);
	}
	if (count == 0)
		type = {};

	return true;
}
