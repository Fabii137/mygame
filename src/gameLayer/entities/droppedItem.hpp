#pragma once

#include <cstdint>

#include "entity.hpp"

class DroppedItem : public Entity {
public:
  DroppedItem(std::uint16_t itemType);

  void render(AssetManager &assetManager) const override;
  bool update(float dt, EntityUpdateData &updateData) override;
  EntityType getType() const override;
  float getMaxHealth() const override;

  // dropped item cant be damaged
  void takeDamage(float damage) override {}

  std::uint16_t getItemType() const;
  int getItemCount() const;
  void setItemCount(int count);

  int getMaxStackSize(std::uint16_t itemType) const;
  Vector2 getSize() const;

private:
  std::uint16_t m_ItemType{};
  int m_ItemCounter{1};
};
