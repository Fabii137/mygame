#include <cstdint>

#include "droppedItem.hpp"

#include "assetManager.hpp"
#include "blocks.hpp"
#include "entity.hpp"
#include "entityHolder.hpp"
#include "helpers.hpp"
#include "items.hpp"
#include "raylib.h"
#include "raymath.h"

DroppedItem::DroppedItem(std::uint16_t itemType)
    : m_ItemType(itemType) {
	m_Physics.transform.w = 0.8f;
	m_Physics.transform.h = 0.8f;
}

void DroppedItem::render(AssetManager& assetManager) const {
	Vector2 size { this->size() };
	Rectangle aabb { getRectangleForEntity(m_Physics.transform, size.x, size.y) };

	Texture2D texture { getTextureForItemType(m_ItemType, assetManager) };
	Rectangle source { getTextureCoordsForItemType(m_ItemType) };

	DrawTexturePro(texture, source, aabb, {}, 0.f, WHITE);
}

bool DroppedItem::update(float dt, EntityUpdateData& updateData) {
	int maxStackSize { this->maxStackSize(m_ItemType) };
	if (m_ItemCount >= maxStackSize) {
		return true;
	}

	for (auto& [key, entity] : updateData.entityHolder.entities) {
		if (key == updateData.ownId) {
			continue;
		}
		if (entity->type() != EntityType::DroppedItem) {
			continue;
		}

		DroppedItem* other { static_cast<DroppedItem*>(entity.get()) };
		if (m_ItemType != other->itemType()) {
			continue;
		}

		bool closeEnough {
			Vector2Distance(position(), other->position()) < 0.7f,
		};
		if (!closeEnough || m_ItemCount > other->itemCount()) {
			continue;
		}

		int total { other->itemCount() + m_ItemCount };
		other->itemCount() = std::min(total, maxStackSize);
		m_ItemCount = total - other->itemCount();
		if (m_ItemCount == 0) {
			// destroy
			return false;
		}
	}
	return true;
}

EntityType DroppedItem::type() const { return EntityType::DroppedItem; }

int DroppedItem::maxStackSize(std::uint16_t itemType) {
	if (isBlock(itemType) || isWall(itemType)) {
		return 64;
	}
	return 1;
}

float DroppedItem::maxHealth() const { return 1.f; }

std::uint16_t DroppedItem::itemType() const { return m_ItemType; }

int& DroppedItem::itemCount() { return m_ItemCount; }

int DroppedItem::itemCount() const { return m_ItemCount; }

Vector2 DroppedItem::size() const {
	if (isBlock(m_ItemType) || isWall(m_ItemType)) {
		return { 1.f, 1.f };
	}

	return { 0.5f, 0.5f };
}
