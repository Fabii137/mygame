#pragma once

#include <vector>

#include "blocks.hpp"
#include "raylib.h"
#include "walls.hpp"
#include "worldGenerator.hpp"

struct MapCell {
  int x{};
  int y{};
  Block *block{};
  Wall *wall{};
};

struct GameMap {
  int w{};
  int h{};
  std::vector<Biome> biomes{};

  std::vector<Block> mapData;
  std::vector<Wall> wallData;

  void create(int w, int h);

  bool inBounds(Vector2 position);

  MapCell getHoveredCell(Vector2 mousePosWorld);

  Block &getBlockUnsafe(int x, int y);
  Block *getBlockSafe(int x, int y);

  Wall &getWallUnsafe(int x, int y);
  Wall *getWallSafe(int x, int y);
};
