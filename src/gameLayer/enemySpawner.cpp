#include <iostream>

#include "enemySpawner.hpp"

#include "random.h"
#include "raymath.h"

void EnemySpawner::update(float dt, EnemySpawnerUpdateData& updateData) {
	updateDespawning(dt, updateData);
	updateSpawning(dt, updateData);
}

void EnemySpawner::updateSpawning(
    float dt, EnemySpawnerUpdateData& updateData) {
	m_EnemySpawnTimer -= dt;

	if (m_EnemySpawnTimer > 0.f) {
		return;
	}

	m_EnemySpawnTimer = ENEMY_SPAWN_INTERVAL;
	if (getEnemiesCount(updateData.entities) >= MAX_ENEMIES) {
		return;
	}

	std::cout << "Spawn attempt\n";

	float distance {
		getRandomFloat(
		    updateData.rng, ENEMY_SPAWN_MIN_DISTANCE, ENEMY_SPAWN_MAX_DISTANCE),
	};
	float direction { getRandomChance(updateData.rng, 0.5f) ? -1.f : 1.f };
	float spawnX { updateData.playerPosition.x + distance * direction };

	auto spawnPosition { findSpawnPosition(
		  spawnX, updateData.gameMap, updateData.playerPosition) };
	if (!spawnPosition.has_value()) {
		return;
	}

	std::cout << "Spawning\n";

	updateData.spawnEnemy(spawnPosition.value());
}

void EnemySpawner::updateDespawning(
    float dt, EnemySpawnerUpdateData& updateData) {
	for (auto& [id, entity] : updateData.entities.entities) {
		if (!entity->isEnemy()) {
			continue;
		}

		float distanceToPlayer { Vector2Distance(
			  entity->position(), updateData.playerPosition) };
		bool inBounds { updateData.gameMap.inBounds(entity->position()) };

		if (distanceToPlayer >= ENEMY_DESPAWN_DISTANCE) {
			entity->timeOutsideDespawnRange() += dt;
		} else {
			entity->timeOutsideDespawnRange() = 0.f;
		}
		bool shouldDespawn {
			entity->timeOutsideDespawnRange() >= ENEMY_DESPAWN_DELAY || !inBounds
		};
		if (shouldDespawn) {
			entity->kill();
		}
	}
}

std::optional<Vector2> EnemySpawner::findSpawnPosition(
    float x, GameMap& gameMap, const Vector2& playerPosition) const {
	int blockX { static_cast<int>(std::floor(x)) };

	if (blockX < 0 || blockX >= gameMap.w) {
		return std::nullopt;
	}

	int playerY { static_cast<int>(std::floor(playerPosition.y)) };
	int startY { std::max(
		  1, playerY - static_cast<int>(ENEMY_SPAWN_MAX_DISTANCE)) };
	int endY { std::min(
		  gameMap.h - 2, playerY + static_cast<int>(ENEMY_SPAWN_MAX_DISTANCE)) };

	for (int y { startY }; y <= endY; ++y) {
		Block* ground { gameMap.blockSafe(blockX, y) };
		Block* above { gameMap.blockSafe(blockX, y - 1) };
		Block* above2 { gameMap.blockSafe(blockX, y - 2) };

		if (!ground || !above || !above2) {
			continue;
		}

		if (ground->type != Block::Air && above->type == Block::Air
		    && above2->type == Block::Air) {
			return Vector2 {
				static_cast<float>(blockX) + 0.5f,
				static_cast<float>(y) - 0.5f,
			};
		}
	}

	return std::nullopt;
}

std::size_t EnemySpawner::getEnemiesCount(EntityHolder& entities) const {
	std::size_t count {};
	for (const auto& [id, entity] : entities.entities) {
		if (entity->isEnemy()) {
			count++;
		}
	}
	return count;
}
