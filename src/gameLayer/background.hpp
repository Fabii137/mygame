#pragma once

#include "raylib.h"
#include "worldGenerator.hpp"

struct AssetManager;

enum class BackgroundType {
  None = 0,
  Forest,
  Desert,
  Snow,
  Cave,
};

struct Background {

  void update(float dt);
  void draw(AssetManager &assetManager, Camera2D camera, Vector2 mapSize);
  void setBackground(BackgroundType background);
  Color getAmbientTint() const;

private:
  void updateDayNight(float dt);
  void drawDayNightBg(AssetManager &assetManager);
  void drawBiomeBg(AssetManager &assetManager, Camera2D camera,
                   Vector2 mapSize);

private:
  float m_TimeOfDay{0.2f};
  float m_CycleDuration{120.f};
  float m_Transition{0.0f};

  BackgroundType m_BiomeBackgroundType{Biome::Forest};
  BackgroundType m_BiomeTransitionType{};
  float m_BiomeTransitionTime{};
};
