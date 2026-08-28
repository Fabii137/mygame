#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <random>

#include "entityHolder.hpp"
#include "gameMap.hpp"
#include "raylib.h"

struct EnemySpawnerUpdateData {
	std::ranlux24_base& rng;
	const Vector2& playerPosition;
	GameMap& gameMap;
	const EntityHolder& entities;
	std::function<void(Vector2)> spawnEnemy;
};

class EnemySpawner {
public:
	void update(float dt, EnemySpawnerUpdateData& updateData);

private:
	void updateSpawning(float dt, EnemySpawnerUpdateData& updateData);
	void updateDespawning(float dt, EnemySpawnerUpdateData& updateData);
	std::optional<Vector2> findSpawnPosition(
	    float x, GameMap& gameMap, const Vector2& playerPosition) const;
	std::size_t getEnemiesCount(const EntityHolder& entities) const;

private:
	static constexpr float ENEMY_SPAWN_INTERVAL { 3.f };
	static constexpr float ENEMY_SPAWN_MIN_DISTANCE { 20.f };
	static constexpr float ENEMY_SPAWN_MAX_DISTANCE { 50.f };
	static constexpr float ENEMY_DESPAWN_DISTANCE { 100.f };
	static constexpr float ENEMY_DESPAWN_DELAY { 10.f };
	static constexpr std::size_t MAX_ENEMIES { 20 };
	float m_EnemySpawnTimer { ENEMY_SPAWN_INTERVAL };
};
