#pragma once

#include <cstdint>

#include <cstddef>

#include "asserts.h"
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
const char* itemName(std::uint16_t type);

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
	size_t count {};

	const char* name() const { return ::itemName(type); }

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

inline const char* blockName(std::uint16_t type) {
	switch (type) {
	case Block::Air:
		return "Air";
	case Block::Dirt:
		return "Dirt";
	case Block::GrassBlock:
		return "Grass Block";
	case Block::Stone:
		return "Stone";
	case Block::Grass:
		return "Grass";
	case Block::Sand:
		return "Sand";
	case Block::SandRuby:
		return "Sand Ruby";
	case Block::SandStone:
		return "Sand Stone";
	case Block::WoodPlank:
		return "Wood Plank";
	case Block::StoneBricks:
		return "Stone Bricks";
	case Block::Clay:
		return "Clay";
	case Block::WoodLog:
		return "Wood Log";
	case Block::Leaves:
		return "Leaves";
	case Block::BloomingLeaves:
		return "Blooming Leaves";
	case Block::Copper:
		return "Copper";
	case Block::Iron:
		return "Iron";
	case Block::Gold:
		return "Gold";
	case Block::CopperBlock:
		return "Copper Block";
	case Block::IronBlock:
		return "Iron Block";
	case Block::GoldBlock:
		return "Gold Block";
	case Block::Bricks:
		return "Bricks";
	case Block::Snow:
		return "Snow";
	case Block::Ice:
		return "Ice";
	case Block::RubyBlock:
		return "Ruby Block";
	case Block::Platform:
		return "Platform";
	case Block::WorkBench:
		return "Work Bench";
	case Block::Glass:
		return "Glass";
	case Block::Furnace:
		return "Furnace";
	case Block::Painting:
		return "Painting";
	case Block::Sappling:
		return "Sappling";
	case Block::SnowBlueRuby:
		return "Snow Blue Ruby";
	case Block::BlueRubyBlock:
		return "Blue Ruby Block";
	case Block::Door:
		return "Door";
	case Block::Jar:
		return "Jar";
	case Block::Table:
		return "Table";
	case Block::Wordrobe:
		return "Wardrobe";
	case Block::BookShelf:
		return "Book Shelf";
	case Block::SnowBricks:
		return "Snow Bricks";
	case Block::IceTable:
		return "Ice Table";
	case Block::IceWordrobe:
		return "Ice Wardrobe";
	case Block::IceBookShelf:
		return "Ice Book Shelf";
	case Block::IcePlatform:
		return "Ice Platform";
	case Block::SandTable:
		return "Sand Table";
	case Block::SandWordrobe:
		return "Sand Wardrobe";
	case Block::SandBookShelf:
		return "Sand Book Shelf";
	case Block::SandPlatform:
		return "Sand Platform";
	case Block::WoodenChest:
		return "Wooden Chest";
	case Block::IceChest:
		return "Ice Chest";
	case Block::SandChest:
		return "Sand Chest";
	case Block::BoneChest:
		return "Bone Chest";
	case Block::BoneBricks:
		return "Bone Bricks";
	case Block::BoneBench:
		return "Bone Bench";
	case Block::BoneWordrobe:
		return "Bone Wardrobe";
	case Block::BoneBookShelf:
		return "Bone Book Shelf";
	case Block::BonePlatform:
		return "Bone Platform";
	case Block::Purpur:
		return "Purpur";
	default:
		return "Unknown Block";
	}
}

inline const char* wallName(std::uint16_t type) {
	switch (type) {
	case Wall::DirtWall:
		return "Dirt Wall";
	case Wall::StoneWall:
		return "Stone Wall";
	case Wall::WoodWall:
		return "Wood Wall";
	case Wall::SandStoneWall:
		return "Sand Stone Wall";
	case Wall::BrickWall:
		return "Brick Wall";
	case Wall::GlassWall:
		return "Glass Wall";
	case Wall::CopperBlockWall:
		return "Copper Block Wall";
	case Wall::SilverBlockWall:
		return "Silver Block Wall";
	case Wall::GoldBlockWall:
		return "Gold Block Wall";
	case Wall::SnowWall:
		return "Snow Wall";
	case Wall::SandWall:
		return "Sand Wall";
	case Wall::StoneBricksWall:
		return "Stone Bricks Wall";
	case Wall::RubyBlockWall:
		return "Ruby Block Wall";
	case Wall::HeroglyphWall:
		return "Heroglyph Wall";
	case Wall::BlueRubyWall:
		return "Blue Ruby Wall";
	case Wall::PlankedWall:
		return "Planked Wall";
	case Wall::SnowBrickWall:
		return "Snow Brick Wall";
	case Wall::BoneBrickWall:
		return "Bone Brick Wall";
	default:
		return "Unknown Wall";
	}
}

inline const char* itemName(std::uint16_t type) {
	if (isBlock(type)) {
		return blockName(type);
	}
	if (isWall(type)) {
		return wallName(type);
	}

	switch (type) {
	case Item::WoodPickaxe:
		return "Wood Pickaxe";
	case Item::WoodAxe:
		return "Wood Axe";
	case Item::WoodHammer:
		return "Wood Hammer";
	case Item::StonePickaxe:
		return "Stone Pickaxe";
	case Item::StoneAxe:
		return "Stone Axe";
	case Item::StoneHammer:
		return "Stone Hammer";
	case Item::CopperPickaxe:
		return "Copper Pickaxe";
	case Item::CopperAxe:
		return "Copper Axe";
	case Item::CopperHammer:
		return "Copper Hammer";
	case Item::IronPickaxe:
		return "Iron Pickaxe";
	case Item::IronAxe:
		return "Iron Axe";
	case Item::IronHammer:
		return "Iron Hammer";
	case Item::GoldPickaxe:
		return "Gold Pickaxe";
	case Item::GoldAxe:
		return "Gold Axe";
	case Item::GoldHammer:
		return "Gold Hammer";
	case Item::Slime:
		return "Slime";
	case Item::CopperIngot:
		return "Copper Ingot";
	case Item::IronIngot:
		return "Iron Ingot";
	case Item::GoldIngot:
		return "Gold Ingot";
	case Item::WoodenSword:
		return "Wooden Sword";
	case Item::StoneSword:
		return "Stone Sword";
	case Item::CopperSword:
		return "Copper Sword";
	case Item::IronSword:
		return "Iron Sword";
	case Item::GoldSword:
		return "Gold Sword";
	case Item::Ruby:
		return "Ruby";
	case Item::CopperSpear:
		return "Copper Spear";
	case Item::IronSpear:
		return "Iron Spear";
	case Item::GoldSpear:
		return "Gold Spear";
	case Item::BlueRuby:
		return "Blue Ruby";
	case Item::IceSpear:
		return "Ice Spear";
	case Item::HealthPotion:
		return "Health Potion";
	case Item::Apple:
		return "Apple";
	case Item::Paper:
		return "Paper";
	case Item::Book:
		return "Book";
	case Item::Bone:
		return "Bone";
	case Item::Shuriken:
		return "Shuriken";
	case Item::CopperDagger:
		return "Copper Dagger";
	case Item::IronDagger:
		return "Iron Dagger";
	case Item::IceDagger:
		return "Ice Dagger";
	case Item::CopperChestPlate:
		return "Copper Chestplate";
	case Item::CopperHelmet:
		return "Copper Helmet";
	case Item::CopperBoots:
		return "Copper Boots";
	case Item::IronChestPlate:
		return "Iron Chestplate";
	case Item::IronHelmet:
		return "Iron Helmet";
	case Item::IronBoots:
		return "Iron Boots";
	case Item::GoldChestPlate:
		return "Gold Chestplate";
	case Item::GoldHelmet:
		return "Gold Helmet";
	case Item::GoldBoots:
		return "Gold Boots";
	case Item::PartyHat:
		return "Party Hat";
	case Item::SunGlasses:
		return "Sun Glasses";
	case Item::IceChestPlate:
		return "Ice Chestplate";
	case Item::IceHelmet:
		return "Ice Helmet";
	case Item::IceBoots:
		return "Ice Boots";
	}

	permaAssertCommentDevelopment(true, "Unknown item type in itemName()");
	return "Unknown";
}
