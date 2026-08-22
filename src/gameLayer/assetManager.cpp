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
  load(items, "items.png");

  // backgrounds
  load(forestMountainsClose, "backgrounds/forest/mountainsClose.png");
  load(forestMountainsFar, "backgrounds/forest/mountainsFar.png");
  load(forestTrees, "backgrounds/forest/trees.png");
  load(desertDunesBack, "backgrounds/desert/dunesBack.png");
  load(desertDunesFront, "backgrounds/desert/dunesFront.png");
  load(snowMountainsBack, "backgrounds/snow/mountainsBack.png");
  load(snowMountainsFront, "backgrounds/snow/mountainsFront.png");
  load(snowTrees, "backgrounds/snow/trees.png");
  load(snowHill, "backgrounds/snow/hill.png");
  load(caveBg, "backgrounds/caveBG.png");
  load(clouds, "backgrounds/sky/clouds.png");
  load(cloudsNight, "backgrounds/sky/cloudsNight.png");
  load(moon, "backgrounds/sky/moon.png");
  load(moonStandalone, "backgrounds/sky/moonStandalone.png");
  load(nightSky, "backgrounds/sky/nightSky.png");
  load(sky, "backgrounds/sky/sky.png");
  load(stars, "backgrounds/sky/stars.png");
  load(sun, "backgrounds/sky/sun.png");
  load(sunStandalone, "backgrounds/sky/sunStandalone.png");
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
