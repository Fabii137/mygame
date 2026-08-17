#pragma once

#include "entity.hpp"

class Player : public Entity {
public:
  Player(float speed, float jumpStrength);

  void render(AssetManager &assetManager) const override;
  bool update(float dt, EntityUpdateData &updateData) override;

  EntityType getType() const override;
  float getMaxHealth() const override;

  float &speed();

private:
  float m_Speed{};
  float m_JumpStrength{};
};
