#pragma once

#include <cstdint>

bool isCollidable(std::uint16_t type);

struct Block {
  enum Type : std::uint16_t {
    Air = 0,
    Dirt,
    GrassBlock,
    Stone,
    Grass,
    Sand,
    SandRuby,
    SandStone,
    WoodPlank,
    StoneBricks,
    Clay,
    WoodLog,
    Leaves,
    BloomingLeaves,
    Copper,
    Iron,
    Gold,
    CopperBlock,
    IronBlock,
    GoldBlock,
    Bricks,
    Snow,
    Ice,
    RubyBlock,
    Platform,
    WorkBench,
    Glass,
    Furnace,
    Painting,
    Sappling,
    SnowBlueRuby,
    BlueRubyBlock,
    Door,
    Jar,
    Table,
    Wordrobe,
    BookShelf,
    SnowBricks,
    IceTable,
    IceWordrobe,
    IceBookShelf,
    IcePlatform,
    SandTable,
    SandWordrobe,
    SandBookShelf,
    SandPlatform,
    WoodenChest,
    IceChest,
    SandChest,
    BoneChest,
    BoneBricks,
    BoneBench,
    BoneWordrobe,
    BoneBookShelf,
    BonePlatform,
    Purpur,

    BLOCKS_COUNT,
    BLOCKS_END = 10000,
  };

  Type type{};

  void sanitize() {
    if (type >= BLOCKS_COUNT) {
      type = Air;
    }
  }

  bool isCollidable() {
    ::isCollidable(type);
    switch (type) {
    case Air:
    case Leaves:
    case BloomingLeaves:
    case Platform:
    case WorkBench:
    case Painting:
    case Sappling:
    case Door:
    case Jar:
    case Table:
    case Wordrobe:
    case BookShelf:
    case IcePlatform:
    case IceTable:
    case IceWordrobe:
    case IceBookShelf:
    case SandPlatform:
    case SandTable:
    case SandWordrobe:
    case SandBookShelf:
    case BonePlatform:
    case BoneBench:
    case BoneWordrobe:
    case BoneBookShelf:
    case WoodLog:
      return false;
    default:
      return true;
    }
  }
};

inline bool isCollidable(std::uint16_t type) {
  switch (type) {
  case Block::Air:
  case Block::Leaves:
  case Block::BloomingLeaves:
  case Block::Platform:
  case Block::WorkBench:
  case Block::Painting:
  case Block::Sappling:
  case Block::Door:
  case Block::Jar:
  case Block::Table:
  case Block::Wordrobe:
  case Block::BookShelf:
  case Block::IcePlatform:
  case Block::IceTable:
  case Block::IceWordrobe:
  case Block::IceBookShelf:
  case Block::SandPlatform:
  case Block::SandTable:
  case Block::SandWordrobe:
  case Block::SandBookShelf:
  case Block::BonePlatform:
  case Block::BoneBench:
  case Block::BoneWordrobe:
  case Block::BoneBookShelf:
  case Block::WoodLog:
    return false;
  default:
    return true;
  }
}
