#pragma once

#include "constants.hpp"
#include "physics.hpp"
#include "raylib.h"

Rectangle getTextureAtlas(int x, int y, int cellSizeX, int cellSizeY);

Rectangle getRectangleForEntity(Transform2D transform, float textureW,
                                float textureH);

void drawTextureAtlas(const Texture2D &texture, int atlasX, int atlasY,
                      Rectangle dest, Color tint = WHITE,
                      int cellSizeX = Constants::CellSize,
                      int cellSizeY = Constants::CellSize);
void drawTexture(const Texture2D &texture, Rectangle src, Rectangle dest,
                 Color tint = WHITE);

Vector2 getMousePosWorld();
