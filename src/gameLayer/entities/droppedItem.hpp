#pragma once

#include <cstdint>

#include "entity.hpp"

class DroppedItem : public Entity {
public:
	DroppedItem(std::uint16_t itemType);

	void render(AssetManager& assetManager) const override;
	bool update(float dt, EntityUpdateData& updateData) override;
	EntityType type() const override;
	float maxHealth() const override;

	// dropped item cant be damaged
	void takeDamage(float damage) override { }

	std::uint16_t itemType() const;
	int& itemCount();
	int itemCount() const;

	int maxStackSize() const;
	Vector2 size() const;

private:
	std::uint16_t m_ItemType {};
	int m_ItemCount { 1 };
};
