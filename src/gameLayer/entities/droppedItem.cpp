#include <cstdint>

#include "droppedItem.hpp"

#include "assetManager.hpp"
#include "blocks.hpp"
#include "entity.hpp"
#include "entityHolder.hpp"
#include "helpers.hpp"
#include "raymath.h"

DroppedItem::DroppedItem(std::uint16_t itemType) : m_ItemType(itemType) {
  m_Physics.transform.w = 0.8f;
  m_Physics.transform.h = 0.8f;
}

void DroppedItem::render(AssetManager &assetManager) const {
  Rectangle aabb{getRectangleForEntity(m_Physics.transform, 1, 1)};

  drawTextureAtlas(assetManager.textures, m_ItemType, 4, aabb);
}

bool DroppedItem::update(float dt, EntityUpdateData &updateData) {
  int maxStackSize{getMaxStackSize(m_ItemType)};
  if (m_ItemCounter >= maxStackSize) {
    return true;
  }

  for (auto &[key, entity] : updateData.entityHolder.entities) {
    if (key == updateData.ownId) {
      continue;
    }
    if (entity->getType() != EntityType::DroppedItem) {
      continue;
    }

    DroppedItem *other{static_cast<DroppedItem *>(entity.get())};
    if (m_ItemType != other->getItemType()) {
      continue;
    }

    bool closeEnough{
        Vector2Distance(position(), other->position()) < 0.7f,
    };
    if (!closeEnough || m_ItemCounter > other->getItemCount()) {
      continue;
    }

    int total{other->getItemCount() + m_ItemCounter};
    other->setItemCount(std::min(total, maxStackSize));
    m_ItemCounter = total - other->getItemCount();
    if (m_ItemCounter == 0) {
      // destroy
      return false;
    }
  }
  return true;
}

EntityType DroppedItem::getType() const { return EntityType::DroppedItem; }

int DroppedItem::getMaxStackSize(std::uint16_t itemType) const {
  if (itemType < Block::BLOCKS_COUNT) {
    return 64;
  }
  return 1;
}

float DroppedItem::getMaxHealth() const { return 1.f; }

std::uint16_t DroppedItem::getItemType() const { return m_ItemType; }
int DroppedItem::getItemCount() const { return m_ItemCounter; }
void DroppedItem::setItemCount(int count) { m_ItemCounter = count; }
