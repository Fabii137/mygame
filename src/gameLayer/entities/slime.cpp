#include "slime.hpp"

#include "assetManager.hpp"
#include "entity.hpp"
#include "helpers.hpp"
#include "random.h"
#include "raymath.h"

Slime::Slime(SlimeType type) : m_Type(type) {
  m_Physics.transform.w = 0.8f;
  m_Physics.transform.h = 0.8f;
  m_Health = getMaxHealth();
}

void Slime::render(AssetManager &assetManager) const {
  Rectangle aabb{getRectangleForEntity(m_Physics.transform, 1, 1)};

  Color tint{isHit() ? ENTITY_HIT_TINT : WHITE};

  switch (m_Type) {
  case SlimeType::Green:
    drawTextureAtlas(assetManager.slime, m_Animation.positionX,
                     m_Animation.positionY, aabb, tint);
    break;
  case SlimeType::Desert:
    drawTextureAtlas(assetManager.desertSlime, m_Animation.positionX,
                     m_Animation.positionY, aabb, tint);
    break;
  case SlimeType::Ice:
    drawTextureAtlas(assetManager.iceSlime, m_Animation.positionX,
                     m_Animation.positionY, aabb, tint);
    break;
  }
}

bool Slime::update(float dt, EntityUpdateData &updateData) {
  updateTimers(dt);
  m_ChangeStateTimer -= dt;

  if (m_ChangeStateTimer < 0.f) {
    m_ChangeStateTimer = getRandomFloat(updateData.rng, 1, 7);

    float distanceToPlayer{
        Vector2Distance(updateData.playerPosition, position())};
    if (distanceToPlayer < 15.f) {
      m_CurrentState = State::Chasing;
    } else {
      m_CurrentState = State::Wandering;
    }
  }

  if (m_Physics.downTouch) {
    m_MoveSpeed = 0.f;
    m_Animation.setAnimation(0);
  } else {
    m_Animation.setAnimation(1);
  }

  m_JumpTimer -= dt;

  switch (m_CurrentState) {
  case State::Wandering: {
    if (m_JumpTimer < 0.f) {
      m_JumpTimer = getRandomFloat(updateData.rng, 3, 12);
      m_Physics.jump(10);
      m_MoveSpeed = getRandomFloat(updateData.rng, -7, 7);
    }
  } break;
  case State::Chasing: {
    if (m_JumpTimer < 0.f) {
      m_JumpTimer = getRandomFloat(updateData.rng, 2, 7);
      m_Physics.jump(10);
      float speed{getRandomFloat(updateData.rng, 1, 7)};
      if (updateData.playerPosition.x > position().x) {
        m_MoveSpeed = speed;
      } else {
        m_MoveSpeed = -speed;
      }
    }
  } break;
  }

  if (m_MoveSpeed) {
    position().x += m_MoveSpeed * dt;
  }

  m_Animation.update(dt, 0.08f, 7);

  return true;
}

EntityType Slime::getType() const { return EntityType::Slime; }

float Slime::getMaxHealth() const {
  switch (m_Type) {
  case SlimeType::Green:
    return 10.f;
  case SlimeType::Ice:
    return 20.f;
  case SlimeType::Desert:
    return 30.f;
  }
}

bool Slime::isEnemy() const { return true; }
