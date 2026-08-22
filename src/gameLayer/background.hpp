#pragma once

#include "raylib.h"

struct AssetManager;

struct Background {
  void update(float dt);
  void draw(AssetManager &assetManager, Camera2D camera, Vector2 mapSize);
};
