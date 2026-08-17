#pragma once

#include <cstdint>

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

  };

  Type type{};

  void sanitize() {
    if (type >= BLOCKS_COUNT) {
      type = Air;
    }
  }

  bool isCollidable() {
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
