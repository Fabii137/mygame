#include <cstdint>

#include <algorithm>

#include "droppedItem.hpp"

#include "assetManager.hpp"
#include "entity.hpp"
#include "entityHolder.hpp"
#include "helpers.hpp"
#include "items.hpp"
#include "raylib.h"
#include "raymath.h"

#include "nlohmann/json.hpp"

DroppedItem::DroppedItem() { setColliderSize(); }

void DroppedItem::render(AssetManager& assetManager) const {
	Vector2 itemSize { size() };
	Rectangle aabb { getRectangleForEntity(
		  m_Physics.transform, itemSize.x, itemSize.y) };

	Texture2D texture { getTextureForItemType(m_ItemType, assetManager) };
	Rectangle source { getTextureCoordsForItemType(m_ItemType) };

	DrawTexturePro(texture, source, aabb, {}, 0.f, WHITE);
}

bool DroppedItem::update(float dt, EntityUpdateData& updateData) {
	int stackSizeLimit { maxStackSize() };
	if (m_ItemCount >= stackSizeLimit) {
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
		other->itemCount() = std::min(total, stackSizeLimit);
		m_ItemCount = total - other->itemCount();
		if (m_ItemCount == 0) {
			// destroy
			return false;
		}
	}
	return true;
}

EntityType DroppedItem::type() const { return EntityType::DroppedItem; }

int DroppedItem::maxStackSize() const {
	if (isBlock(m_ItemType) || isWall(m_ItemType)) {
		return 64;
	}
	return 1;
}

float DroppedItem::maxHealth() const { return 1.f; }

std::uint16_t& DroppedItem::itemType() { return m_ItemType; }

std::uint16_t DroppedItem::itemType() const { return m_ItemType; }

int& DroppedItem::itemCount() { return m_ItemCount; }

int DroppedItem::itemCount() const { return m_ItemCount; }

Vector2 DroppedItem::size() const {
	if (isBlock(m_ItemType) || isWall(m_ItemType)) {
		return { 1.f, 1.f };
	}

	return { 0.5f, 0.5f };
}

void DroppedItem::setColliderSize() {
	m_Physics.transform.w = 0.8f;
	m_Physics.transform.h = 0.8f;
}

Json DroppedItem::formatToJson() const {
	Json json {};
	addEntityCommonToJson(json);

	json["itemType"] = m_ItemType;
	json["itemCount"] = m_ItemCount;

	return json;
}

bool DroppedItem::loadFromJson(Json& json) {
	*this = {};

	if (!loadEntityCommonFromJson(json)) {
		return false;
	}

	if (!json.contains("itemType") || !json["itemType"].is_number_unsigned()) {
		return false;
	}
	m_ItemType = json["itemType"];
	if (m_ItemType >= Item::LAST_ITEM) {
		return false;
	}

	if (!json.contains("itemCount") || !json["itemCount"].is_number()) {
		return false;
	}
	m_ItemCount = json["itemCount"];
	m_ItemCount = std::clamp(m_ItemCount, 0, maxStackSize());

	return true;
}
