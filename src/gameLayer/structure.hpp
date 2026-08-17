#pragma once

#include <vector>

#include "blocks.hpp"
#include "gameMap.hpp"
#include "raylib.h"
#include "walls.hpp"

struct Structure {

  int w{};
  int h{};

  std::vector<Block> mapData;
  std::vector<Wall> wallData;

  void create(int w, int h);

  Block &getBlockUnsafe(int x, int y);
  Block *getBlockSafe(int x, int y);

  Wall &getWallUnsafe(int x, int y);
  Wall *getWallSafe(int x, int y);

  void copyFromMap(GameMap &map, Vector2 start, Vector2 end);
  void pasteIntoMap(GameMap &map, Vector2 start);
};
