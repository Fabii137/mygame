#pragma once

#include "constants.hpp"
#include "physics.hpp"
#include "raylib.h"

Rectangle getTextureAtlas(
    int x, int y, int cellSizeX, int cellSizeY, bool flipX = false);

Rectangle flipTextureAtlasX(Rectangle rect);

Rectangle getRectangleForEntity(
    Transform2D transform, float textureW, float textureH);

Rectangle shrinkUV(Rectangle rect, float shrink = 0.1f);

void drawTextureAtlas(const Texture2D& texture, int atlasX, int atlasY,
    Rectangle dest, Color tint = WHITE, int cellSizeX = Constants::CellSize,
    int cellSizeY = Constants::CellSize);
void drawTexture(const Texture2D& texture, Rectangle src, Rectangle dest,
    Color tint = WHITE);
