#include <algorithm>

#include "background.hpp"

#include "assetManager.hpp"
#include "raylib.h"

void Background::update(float dt) {}

void Background::draw(AssetManager &assetManager, Camera2D camera,
                      Vector2 mapSize) {
  auto drawBackground = [&](float parallax, float opacity) {
    Texture bg = assetManager.forestBg;

    int scrWidth{GetScreenWidth()};
    int scrHeight{GetScreenHeight()};

    float aspectRatio{static_cast<float>(scrWidth) /
                      static_cast<float>(scrHeight)};
    constexpr float bgScaleScreen{2.f};

    float base{std::max(scrWidth, scrHeight) * bgScaleScreen};

    float bgWidth{base};
    float bgHeight{base};

    if (aspectRatio > 1.f) {
      bgHeight = bgWidth / aspectRatio;
    } else {
      bgWidth = bgHeight * aspectRatio;
    }

    // camera ranges
    float halfViewW{(scrWidth * 0.5f) / camera.zoom};
    float halfViewH{(scrHeight * 0.5f) / camera.zoom};

    float camMinX{halfViewW};
    float camMaxX{mapSize.x - halfViewW};
    float camMinY{halfViewH};
    float camMaxY{mapSize.y - halfViewH};

    Vector2 camPos{camera.target};
    if (camMinX < camMaxX) {
      camPos.x = std::clamp(camPos.x, camMinX, camMaxX);
    } else {
      camPos.x = mapSize.x * 0.5f;
    }

    if (camMinY < camMaxY) {
      camPos.y = std::clamp(camPos.y, camMinY, camMaxY);
    } else {
      camPos.y = mapSize.y * 0.5f;
    }

    float camRangeX{std::max(0.f, camMaxX - camMinX)};
    float camRangeY{std::max(0.f, camMaxY - camMinY)};

    float normX{camRangeX > 0.f ? (camPos.x - camMinX) / camRangeX : 0.f};
    float normY{camRangeY > 0.f ? (camPos.y - camMinY) / camRangeY : 0.f};

    // how far the bg can move inside the screen
    float maxOffsetX{bgWidth - scrWidth};
    float maxOffsetY{bgHeight - scrHeight};

    float offsetX{-maxOffsetX * normX * parallax};
    float offsetY{-maxOffsetY * normY * parallax};

    Rectangle src{
        0.f,
        0.f,
        static_cast<float>(bg.width),
        static_cast<float>(bg.height),
    };

    Rectangle dest{offsetX, offsetY, bgWidth, bgHeight};

    DrawTexturePro(bg, src, dest, {}, 0.f, Fade(WHITE, opacity));
  };

  drawBackground(0.3f, 1.f);
}
