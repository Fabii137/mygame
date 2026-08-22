#pragma once

#include <cstdint>

#include <random>

#include "assetManager.hpp"
#include "background.hpp"
#include "entityHolder.hpp"
#include "gameMap.hpp"
#include "raylib.h"
#include "structure.hpp"
#include "worldGenerator.hpp"

#include "entities/player.hpp"
#include "entities/slime.hpp"

class Game {
public:
  bool init();
  bool update();
  void close();

private:
  void spawnZombie(Vector2 position);
  void spawnSlime(Vector2 position, SlimeType type);
  void spawnDroppedItem(Vector2 position, std::uint16_t type);
  void updateAudio(float dt);
  void updatePlayer(float dt);
  void updateCamera();
  void updateEntities(float dt);
  void updateEnemySpawning(float dt);
  void updateWorldEditing();
  void updateStructureSelection();
  void render();
  void renderBackground();
  int getTextureVariant(int x, int y);
  int getTreeAtlasColumn(int x, int y);
  std::optional<Vector2> findGroundSpawnPosition(float x);
  Vector2 getMousePosWorld() const;
  void renderImGuiWindows();

private:
  enum class EditMode { Blocks, Walls };

  GameMap m_GameMap{};
  Camera2D m_Camera{};
  Background m_Background{};
  WorldSettings m_WorldSettings{};
  std::ranlux24_base m_Rng{std::random_device{}()};
  std::uint32_t m_Seed{1234};

  bool m_PlayerGravity{true};

  static constexpr float EnemySpawnInterval{3.f};
  static constexpr float EnemySpawnMinDistance{20.f};
  static constexpr float EnemySpawnMaxDistance{50.f};
  static constexpr float EnemyDespawnDistance{100.f};
  static constexpr float EnemyDespawnDelay{10.f};
  static constexpr std::size_t MaxEnemies{20};
  float m_EnemySpawnTimer{};

  EditMode m_EditMode{EditMode::Blocks};
  Block::Type m_CreativeSelectedBlock{Block::Dirt};
  Wall::Type m_CreativeSelectedWall{Wall::DirtWall};

  Vector2 m_SelectionStart{};
  Vector2 m_SelectionEnd{};
  Structure m_CopyStructure{};
  char m_SaveName[100]{};

  static constexpr float PlayerSpeed{10.f};
  static constexpr float PlayerJumpStrength{10.f};
  Player m_Player{PlayerSpeed, PlayerJumpStrength};
  EntityHolder m_Entities{};
  AssetManager m_AssetManager{};

  bool m_ShowImGui{};
};
