#pragma once

#include <filesystem>

#include "raylib.h"

struct AssetManager {
  Texture2D textures{};
  Texture2D frame{};
  Texture2D treeTextures{};
  Texture2D player{};
  Texture2D slime{};
  Texture2D desertSlime{};
  Texture2D iceSlime{};
  Texture2D zombie{};
  Texture2D items{};

  void loadAll(const char *texturePackPath = nullptr);

private:
  void unloadTexture(Texture2D &texture);
  Texture2D loadTexture(const std::filesystem::path &defaultPath,
                        const std::filesystem::path *texturePackPath);
};
