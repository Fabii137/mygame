#pragma once

#include <cstdint>

#include <random>

#include "physics.hpp"
#include "raylib.h"

#include "nlohmann/json_fwd.hpp"

constexpr Color ENTITY_HIT_TINT { 255, 180, 180, 255 };
constexpr float HIT_TIMER_DURATION { 0.5f };
constexpr float RED_TIMER_DURATION { 0.5f };

struct GameMap;
struct EntityHolder;
struct AssetManager;

using Json = nlohmann::json;

enum class EntityType { Player, Slime, DroppedItem, Zombie };

struct EntityUpdateData {
	Vector2 playerPosition {};
	std::ranlux24_base& rng;
	const EntityHolder& entityHolder;
	std::uint64_t ownId {};
	bool creativeMode {};
};

class Entity {
public:
	virtual ~Entity() = default;

	Vector2& position();
	const Vector2& position() const;
	float health() const;
	PhysicalEntity& physics();
	float& timeOutsideDespawnRange();

	void teleport(Vector2 pos);

	virtual void takeDamage(float damage);
	virtual void heal(float amount);

	virtual void render(AssetManager& assetManager) const = 0;
	virtual bool update(float dt, EntityUpdateData& updateData) = 0;

	virtual void setColliderSize() = 0;

	virtual EntityType type() const = 0;
	virtual float maxHealth() const = 0;

	virtual void updatePhysics(
	    float dt, GameMap& gameMap, bool applyGravity = true);

	virtual bool isEnemy() const;
	bool alive() const;
	virtual void kill();

	virtual Json formatToJson() const = 0;
	virtual bool loadFromJson(Json& json) = 0;

protected:
	void updateTimers(float dt);

	void addEntityCommonToJson(Json& json) const;

	bool loadEntityCommonFromJson(Json& json);

	bool isHit() const;

protected:
	PhysicalEntity m_Physics {};
	float m_Health { 1.f };

private:
	float m_RedTimer {};
	float m_HitTimer {};
	float m_TimeOutsideDespawnRange {};
};
