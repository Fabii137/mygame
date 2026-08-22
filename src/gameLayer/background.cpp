#include <algorithm>

#include "background.hpp"

#include "assetManager.hpp"
#include "raylib.h"

constexpr float BIOME_TRANSITION_DURATION{1.f};

void Background::update(float dt) {
  updateDayNight(dt);
  m_BiomeTransitionTime -= dt;
}

Color Background::getAmbientTint() const {
  float daylight{1.f - m_Transition * 0.85f};
  float nightBlend{1.f - daylight};

  unsigned char alpha{static_cast<unsigned char>(nightBlend * 210.f)};
  return Color{20, 24, 48, alpha};
}

void Background::draw(AssetManager &assetManager, Camera2D camera,
                      Vector2 mapSize) {
  drawDayNightBg(assetManager);
  drawBiomeBg(assetManager, camera, mapSize);
}

void Background::setBackground(BackgroundType background) {
  if (background == m_BiomeBackgroundType) {
    return;
  }

  if (m_BiomeTransitionTime <= 0.f) {
    m_BiomeTransitionTime = BIOME_TRANSITION_DURATION;
    m_BiomeTransitionType = m_BiomeBackgroundType;
    m_BiomeBackgroundType = background;
  }
}

void Background::updateDayNight(float dt) {
  m_TimeOfDay = std::fmod(m_TimeOfDay + dt / m_CycleDuration, 1.f);

  constexpr float sunriseStart{0.0f};
  constexpr float sunriseEnd{0.14f};
  constexpr float dayStart{0.14f};
  constexpr float dayEnd{0.72f};
  constexpr float sunsetStart{0.72f};
  constexpr float sunsetEnd{0.86f};

  float phase{m_TimeOfDay};

  if (phase >= sunriseStart && phase < sunriseEnd) {
    m_Transition =
        1.f - std::clamp((phase - sunriseStart) / (sunriseEnd - sunriseStart),
                         0.f, 1.f);
  } else if (phase >= dayStart && phase < dayEnd) {
    m_Transition = 0.f;
  } else if (phase >= sunsetStart && phase < sunsetEnd) {
    m_Transition =
        std::clamp((phase - sunsetStart) / (sunsetEnd - sunsetStart), 0.f, 1.f);
  } else {
    m_Transition = 1.f;
  }
}

void Background::drawDayNightBg(AssetManager &assetManager) {
  int scrWidth{GetScreenWidth()};
  int scrHeight{GetScreenHeight()};
  float daylight{1.f - m_Transition * 0.85f};
  float nightBlend{1.f - daylight};

  Rectangle skySrc{0.f, 0.f, static_cast<float>(assetManager.sky.width),
                   static_cast<float>(assetManager.sky.height)};
  Rectangle skyDest{0.f, 0.f, static_cast<float>(scrWidth),
                    static_cast<float>(scrHeight)};

  DrawTexturePro(assetManager.sky, skySrc, skyDest, {}, 0.f,
                 Fade(WHITE, 1.f - nightBlend * 0.75f));
  DrawTexturePro(assetManager.nightSky, skySrc, skyDest, {}, 0.f,
                 Fade(WHITE, nightBlend));

  float starAlpha{std::clamp(nightBlend * 1.1f, 0.f, 1.f)};
  if (starAlpha > 0.05f) {
    DrawTexturePro(assetManager.stars, skySrc, skyDest, {}, 0.f,
                   Fade(WHITE, starAlpha));
  }

  constexpr float celestialScale{4.f};
  float sunX{scrWidth * 0.2f};
  float sunY{scrHeight * 0.22f};
  float moonX{scrWidth * 0.78f};
  float moonY{scrHeight * 0.26f};

  float sunWidth{static_cast<float>(assetManager.sun.width) * celestialScale};
  float sunHeight{static_cast<float>(assetManager.sun.height) * celestialScale};
  float moonWidth{static_cast<float>(assetManager.moon.width) * celestialScale};
  float moonHeight{static_cast<float>(assetManager.moon.height) *
                   celestialScale};

  Rectangle sunRect{sunX - sunWidth * 0.5f, sunY - sunHeight * 0.5f, sunWidth,
                    sunHeight};
  DrawTexturePro(assetManager.sun,
                 {0.f, 0.f, static_cast<float>(assetManager.sun.width),
                  static_cast<float>(assetManager.sun.height)},
                 sunRect, {}, 0.f,
                 Fade(WHITE, std::clamp(daylight * 1.2f, 0.f, 1.f)));

  Rectangle moonRect{moonX - moonWidth * 0.5f, moonY - moonHeight * 0.5f,
                     moonWidth, moonHeight};
  DrawTexturePro(assetManager.moon,
                 {0.f, 0.f, static_cast<float>(assetManager.moon.width),
                  static_cast<float>(assetManager.moon.height)},
                 moonRect, {}, 0.f,
                 Fade(WHITE, std::clamp(nightBlend * 1.2f, 0.f, 1.f)));

  if (daylight > 0.2f) {
    DrawTexturePro(assetManager.clouds, skySrc, skyDest, {}, 0.f,
                   Fade(WHITE, 0.35f + daylight * 0.55f));
  } else {
    DrawTexturePro(assetManager.cloudsNight, skySrc, skyDest, {}, 0.f,
                   Fade(WHITE, 0.25f + nightBlend * 0.75f));
  }
}

void Background::drawBiomeBg(AssetManager &assetManager, Camera2D camera,
                             Vector2 mapSize) {
  auto drawBackground = [&](Texture2D &texture, float parallax, float opacity) {
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
        static_cast<float>(texture.width),
        static_cast<float>(texture.height),
    };

    Rectangle dest{offsetX, offsetY, bgWidth, bgHeight};

    DrawTexturePro(texture, src, dest, {}, 0.f, Fade(WHITE, opacity));
  };

  auto drawBiomeBackground = [&](BackgroundType background, float opacity) {
    switch (background) {
    case BackgroundType::None:
      break;
    case BackgroundType::Forest:
      drawBackground(assetManager.forestMountainsFar, 0.3f, opacity);
      drawBackground(assetManager.forestMountainsClose, 0.4f, opacity);
      drawBackground(assetManager.forestTrees, 0.7f, opacity);
      break;
    case BackgroundType::Desert:
      drawBackground(assetManager.desertDunesBack, 0.4f, opacity);
      drawBackground(assetManager.desertDunesFront, 0.7f, opacity);
      break;
    case BackgroundType::Snow:
      drawBackground(assetManager.snowMountainsBack, 0.2f, opacity);
      drawBackground(assetManager.snowMountainsFront, 0.4f, opacity);
      drawBackground(assetManager.snowTrees, 0.7f, opacity);
      drawBackground(assetManager.snowHill, 0.8f, opacity);
      break;
    case BackgroundType::Cave:
      drawBackground(assetManager.caveBg, 0.7f, opacity);
      break;
    }
  };

  drawBiomeBackground(m_BiomeBackgroundType, 1.f);
  if (m_BiomeTransitionTime > 0.f) {
    float opacity{m_BiomeTransitionTime};
    if (opacity > 1.f) {
      opacity = 1.f;
    }
    drawBiomeBackground(m_BiomeTransitionType, opacity);
  }
}
