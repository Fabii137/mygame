#include "helpers.hpp"

#include "physics.hpp"
#include "raylib.h"

Rectangle getTextureAtlas(int x, int y, int cellSizeX, int cellSizeY) {
  return {
      static_cast<float>(x) * cellSizeX,
      static_cast<float>(y) * cellSizeY,
      static_cast<float>(cellSizeX),
      static_cast<float>(cellSizeY),
  };
}

Rectangle getRectangleForEntity(Transform2D transform, float textureW,
                                float textureH) {
  Transform2D result{transform};
  result.w = textureW;
  result.h = textureH;

  result.pos.y -= (result.h - transform.h) / 2.f;

  return result.getAABB();
}

void drawTextureAtlas(const Texture2D &texture, int atlasX, int atlasY,
                      Rectangle dest, Color tint, int cellSizeX,
                      int cellSizeY) {
  DrawTexturePro(texture, getTextureAtlas(atlasX, atlasY, cellSizeX, cellSizeY),
                 dest, {}, 0.f, tint);
}

void drawTexture(const Texture2D &texture, Rectangle src, Rectangle dest,
                 Color tint) {
  DrawTexturePro(texture, src, dest, {}, 0.f, tint);
}
