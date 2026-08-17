#pragma once

#include <cstdint>

#include <algorithm>
#include <random>

#include "physics.hpp"
#include "raylib.h"

constexpr Color ENTITY_HIT_TINT{255, 180, 180, 255};
constexpr float HIT_TIMER_DURATION{0.5f};
constexpr float RED_TIMER_DURATION{0.5f};

struct GameMap;
struct EntityHolder;
struct AssetManager;

enum class EntityType { Player, Slime, DroppedItem, Zombie };

struct EntityUpdateData {
  Vector2 playerPosition{};
  std::ranlux24_base &rng;
  EntityHolder &entityHolder;
  std::uint64_t ownId{};
};

class Entity {
public:
  virtual ~Entity() = default;

  Vector2 &position() { return m_Physics.transform.pos; }
  float getHealth() const { return m_Health; }
  PhysicalEntity &physics() { return m_Physics; }
  float &timeOutsideDespawnRange() { return m_TimeOutsideDespawnRange; }

  void teleport(Vector2 pos) { m_Physics.teleport(pos); }

  virtual void takeDamage(float damage) {
    if (m_HitTimer > 0.f) {
      return;
    }

    m_Health = std::max(m_Health - damage, 0.f);
    m_RedTimer = RED_TIMER_DURATION;
    m_HitTimer = HIT_TIMER_DURATION;
  };

  virtual void render(AssetManager &assetManager) const = 0;
  virtual bool update(float dt, EntityUpdateData &updateData) = 0;

  virtual EntityType getType() const = 0;
  virtual float getMaxHealth() const = 0;

  virtual void updatePhysics(float dt, GameMap &gameMap,
                             bool applyGravity = true) {
    if (applyGravity) {
      m_Physics.applyGravity();
    }
    m_Physics.updateForces(dt);
    m_Physics.resolveConstraints(gameMap);
    m_Physics.updateFinal();
  }

  virtual bool isEnemy() const { return false; }

protected:
  void updateTimers(float dt) {
    m_RedTimer = std::max(m_RedTimer - dt, 0.f);
    m_HitTimer = std::max(m_HitTimer - dt, 0.f);
  }

  bool isHit() const { return m_RedTimer > 0.f; }

protected:
  PhysicalEntity m_Physics{};
  float m_Health{1.f};

private:
  float m_RedTimer{};
  float m_HitTimer{};
  float m_TimeOutsideDespawnRange{};
};
