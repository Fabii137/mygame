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
	void updateBackground(float dt);
	void render();
	void renderBackground();
	int getTextureVariant(int x, int y);
	int getTreeAtlasColumn(int x, int y);
	std::optional<Vector2> findGroundSpawnPosition(float x);
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

	static constexpr float ENEMY_SPAWN_INTERVAL { 3.f };
	static constexpr float ENEMY_SPAWN_MIN_DISTANCE { 20.f };
	static constexpr float ENEMY_SPAWN_MAX_DISTANCE { 50.f };
	static constexpr float ENEMY_DESPAWN_DISTANCE { 100.f };
	static constexpr float ENEMY_DESPAWN_DELAY { 10.f };
	static constexpr std::size_t MAX_ENEMIES { 20 };
	float m_EnemySpawnTimer {};

	bool m_CreativeMode {};
	EditMode m_EditMode { EditMode::Blocks };
	Block::Type m_CreativeSelectedBlock { Block::Dirt };
	Wall::Type m_CreativeSelectedWall { Wall::DirtWall };

	Vector2 m_SelectionStart {};
	Vector2 m_SelectionEnd {};
	Structure m_CopyStructure {};
	char m_SaveName[100] {};

	static constexpr float PLAYER_SPEED { 10.f };
	static constexpr float PLAYER_JUMP_STRENGTH { 10.f };
	Player m_Player { PLAYER_SPEED, PLAYER_JUMP_STRENGTH };
	EntityHolder m_Entities {};
	AssetManager m_AssetManager {};

	bool m_ShowImGui {};
};
