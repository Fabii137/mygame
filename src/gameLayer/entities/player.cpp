#include "player.hpp"

#include "assetManager.hpp"
#include "helpers.hpp"
#include "raylib.h"

Player::Player(float speed, float jumpStrength)
    : m_Speed(speed), m_JumpStrength(jumpStrength) {
  m_Physics.transform.w = 0.8f;
  m_Physics.transform.h = 1.6f;
  m_Health = getMaxHealth();
}

void Player::render(AssetManager &assetManager) const {
  Rectangle aabb{m_Physics.transform.getAABB()};

  Color tint{isHit() > 0.f ? RED : WHITE};
  Rectangle src{
      0.f,
      0.f,
      static_cast<float>(assetManager.player.width),
      static_cast<float>(assetManager.player.height),
  };
  drawTexture(assetManager.player, src, aabb, tint);

  DrawRectangleLinesEx(aabb, 0.1f, {20, 101, 250, 120});
}

bool Player::update(float dt, EntityUpdateData &updateData) {
  updateTimers(dt);

  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
    position().x -= m_Speed * dt;
  }
  if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
    position().x += m_Speed * dt;
  }
  if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
    position().y -= m_Speed * dt;
  }
  if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
    position().y += m_Speed * dt;
  }
  if (IsKeyDown(KEY_SPACE)) {
    physics().jump(m_JumpStrength);
  }

  return true;
}

EntityType Player::getType() const { return EntityType::Player; }

float Player::getMaxHealth() const { return 10.f; }

float &Player::speed() { return m_Speed; }
