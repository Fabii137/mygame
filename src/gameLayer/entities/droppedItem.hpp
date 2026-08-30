#pragma once

#include <cstdint>

#include "entity.hpp"

class DroppedItem : public Entity {
public:
	DroppedItem();

	void render(AssetManager& assetManager) const override;
	bool update(float dt, EntityUpdateData& updateData) override;
	EntityType type() const override;
	float maxHealth() const override;

	// dropped item can't be damaged
	void takeDamage(float damage) override { }

	std::uint16_t& itemType();
	std::uint16_t itemType() const;
	size_t& itemCount();
	size_t itemCount() const;

	Vector2 size() const;

	void setColliderSize() override;

	Json formatToJson() const override;
	bool loadFromJson(Json& json) override;

private:
	std::uint16_t m_ItemType {};
	size_t m_ItemCount { 1 };
};
