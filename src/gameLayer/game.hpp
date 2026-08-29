#pragma once

#include <cstdint>

#include <random>

#include "assetManager.hpp"
#include "background.hpp"
#include "enemySpawner.hpp"
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
	void updateBackground(float dt);
	void render();
	void renderBackground();
	void renderPlayerHearts();
	int getTextureVariant(int x, int y);
	int getTreeAtlasColumn(int x, int y);
	Vector2 getMousePosWorld() const;
	bool canPlaceBlock(const MapCell& hoveredCell);
	void renderImGuiWindows();

private:
	enum class EditMode { Blocks, Walls };

	GameMap m_GameMap {};
	Camera2D m_Camera {};
	Background m_Background {};
	WorldSettings m_WorldSettings {};
	std::ranlux24_base m_Rng { std::random_device {}() };
	std::uint32_t m_Seed { 1234 };

	bool m_CreativeMode {};
	EditMode m_EditMode { EditMode::Blocks };
	Block::Type m_CreativeSelectedBlock { Block::Dirt };
	Wall::Type m_CreativeSelectedWall { Wall::DirtWall };

	Vector2 m_SelectionStart {};
	Vector2 m_SelectionEnd {};
	Structure m_CopyStructure {};
	char m_SaveName[100] {};

	Player m_Player {};
	EntityHolder m_Entities {};
	EnemySpawner m_EnemySpawner {};
	AssetManager m_AssetManager {};

	bool m_ShowImGui {};
};
