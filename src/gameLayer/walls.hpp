#pragma once

#include <cstdint>

#include "blocks.hpp"

struct Wall {
  enum Type : std::uint16_t {
    DirtWall = Block::BLOCKS_COUNT,
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
    HeroglyphWall,
    BlueRubyWall,
    PlankedWall,
    SnowBrickWall,
    BoneBrickWall,

    WALLS_COUNT,
  };

  Type type{};

  void sanitize() {
    if (type >= WALLS_COUNT ||
        (type <= static_cast<Type>(Block::BLOCKS_COUNT) &&
         type != static_cast<Type>(Block::Air))) {
      type = static_cast<Type>(Block::Air);
    }
  }
};
