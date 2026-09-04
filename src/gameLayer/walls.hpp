#pragma once

#include <cstdint>

#include "blocks.hpp"

struct Wall {
	enum Type : std::uint16_t {
		DirtWall = Block::BLOCKS_END,
		StoneWall,
		WoodWall,
		SandStoneWall,
		BrickWall,
		GlassWall,
		CopperBlockWall,
		SilverBlockWall,
		GoldBlockWall,
		SnowWall,
		SandWall,
		StoneBricksWall,
		RubyBlockWall,
		HieroglyphWall,
		BlueRubyWall,
		PlankedWall,
		SnowBrickWall,
		BoneBrickWall,

		WALLS_COUNT,
		WALLS_END = 20000,
	};

	Type type {};

	void sanitize() {
		if (type >= WALLS_COUNT
		    || (type <= static_cast<Type>(Block::BLOCKS_COUNT)
		        && type != static_cast<Type>(Block::Air))) {
			type = static_cast<Type>(Block::Air);
		}
	}
};
