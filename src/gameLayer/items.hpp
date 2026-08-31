#pragma once

#include <cstdint>

#include "blocks.hpp"
#include "raylib.h"
#include "walls.hpp"

#include "nlohmann/json_fwd.hpp"

struct AssetManager;
using Json = nlohmann::json;

bool isItem(std::uint16_t type);
bool isBlock(std::uint16_t type);
bool isWall(std::uint16_t type);
size_t maxStackSize(std::uint16_t type);

struct Item {
	enum Type : std::uint16_t {
		WoodPickaxe = Wall::WALLS_END,
		WoodAxe,
		WoodHammer,
		StonePickaxe,
		StoneAxe,
		StoneHammer,
		CopperPickaxe,
		CopperAxe,
		CopperHammer,
		IronPickaxe,
		IronAxe,
		IronHammer,
		GoldPickaxe,
		GoldAxe,
		GoldHammer,
		Slime,
		CopperIngot,
		IronIngot,
		GoldIngot,
		WoodenSword,
		StoneSword,
		CopperSword,
		IronSword,
		GoldSword,
		Ruby,
		CopperSpear,
		IronSpear,
		GoldSpear,
		BlueRuby,
		IceSpear,
		HealthPotion,
		Apple,
		Paper,
		Book,
		Bone,
		Shuriken,
		CopperDagger,
		IronDagger,
		IceDagger,
		CopperChestPlate,
		CopperHelmet,
		CopperBoots,

		IronChestPlate,
		IronHelmet,
		IronBoots,

		GoldChestPlate,
		GoldHelmet,
		GoldBoots,

		PartyHat,
		SunGlasses,

		IceChestPlate,
		IceHelmet,
		IceBoots,

		LAST_ITEM,
		ITEMS_END = 30000,
	};

	Type type {};
	size_t count { 1 };

	size_t maxStackSize() const { return ::maxStackSize(type); }

	bool isItem() const { return ::isItem(type); }
	bool isBlock() const { return ::isBlock(type); }
	bool isWall() const { return ::isWall(type); }

	bool empty() const { return !type || !count; }

	Json formatToJson() const;
	bool loadFromJson(Json& json);
};

Texture getTextureForItemType(
    std::uint16_t type, const AssetManager& assetManager);
Rectangle getTextureCoordsForItemType(std::uint16_t type);

inline size_t maxStackSize(std::uint16_t type) {
	if (isBlock(type) || isWall(type)) {
		return 64;
	}
	return 1;
}

inline bool isItem(std::uint16_t type) {
	return type >= Wall::WALLS_END && type < Item::LAST_ITEM;
}
inline bool isWall(std::uint16_t type) {
	return type >= Block::BLOCKS_END && type < Wall::WALLS_COUNT;
}

inline bool isBlock(std::uint16_t type) { return type < Block::BLOCKS_COUNT; }

inline bool isTool(std::uint16_t type) {
	switch (type) {
	case Item::WoodPickaxe:
	case Item::WoodAxe:
	case Item::WoodHammer:
	case Item::StonePickaxe:
	case Item::StoneAxe:
	case Item::StoneHammer:
	case Item::CopperPickaxe:
	case Item::CopperAxe:
	case Item::CopperHammer:
	case Item::IronPickaxe:
	case Item::IronAxe:
	case Item::IronHammer:
	case Item::GoldPickaxe:
	case Item::GoldAxe:
	case Item::GoldHammer:
		return true;
	}

	return false;
}

inline bool isWeapon(std::uint16_t type) {
	switch (type) {
	case Item::WoodenSword:
	case Item::StoneSword:
	case Item::CopperSword:
	case Item::IronSword:
	case Item::GoldSword:
	case Item::CopperSpear:
	case Item::IronSpear:
	case Item::GoldSpear:
	case Item::IceSpear:
	case Item::CopperDagger:
	case Item::IronDagger:
	case Item::IceDagger:
		return true;
	}
	return false;
}

inline bool isArmor(std::uint16_t type) {
	switch (type) {
	case Item::IronChestPlate:
	case Item::IronHelmet:
	case Item::IronBoots:
	case Item::GoldChestPlate:
	case Item::GoldHelmet:
	case Item::GoldBoots:
	case Item::PartyHat:
	case Item::SunGlasses:
	case Item::IceChestPlate:
	case Item::IceHelmet:
	case Item::IceBoots:
		return true;
	}
	return false;
}
