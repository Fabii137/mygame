#include <filesystem>
#include <string>

#include "assetManager.hpp"

#include "raylib.h"

namespace fs = std::filesystem;

void AssetManager::loadAll(const char *texturePackPath) {
  auto load = [&](Texture2D &texture, const std::string &file) {
    fs::path defaultPath{fs::path(RESOURCES_PATH) / file};

    unloadTexture(texture);
    if (texturePackPath) {
      fs::path packPath{fs::path(texturePackPath) / file};
      texture = loadTexture(defaultPath, &packPath);
      return;
    }

    texture = loadTexture(defaultPath, nullptr);
  };

  load(textures, "texturesWithBackgroundVersion.png");
  load(frame, "frame.png");
  load(treeTextures, "treetextures.png");
  load(player, "player.png");
  load(slime, "slime.png");
  load(desertSlime, "desertslime.png");
  load(iceSlime, "iceslime.png");
  load(zombie, "zombie.png");
}

void AssetManager::unloadTexture(Texture2D &texture) {
  if (IsTextureValid(texture)) {
    UnloadTexture(texture);
  }

  texture = {};
}

Texture2D AssetManager::loadTexture(const fs::path &defaultPath,
                                    const fs::path *packPath) {
  if (packPath && fs::exists(*packPath)) {
    return LoadTexture(packPath->string().c_str());
  }

  return LoadTexture(defaultPath.string().c_str());
}
