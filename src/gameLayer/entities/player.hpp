#pragma once

#include "entity.hpp"
#include "entityAnimation.hpp"
#include "items.hpp"

class Player : public Entity {
public:
	Player(float speed, float jumpStrength);

	void render(AssetManager& assetManager) const override;
	bool update(float dt, EntityUpdateData& updateData) override;

	EntityType type() const override;
	float maxHealth() const override;

	float& speed();
	float speed() const;

private:
	void renderHeldItem(AssetManager& assetManager, Rectangle aabb) const;

private:
	EntityAnimation m_Animation {};

	Item::Type m_ArmorHead { Item::PartyHat };
	Item::Type m_ArmorChest { Item::GoldChestPlate };
	Item::Type m_ArmorLegs { Item::IceBoots };
	Item::Type m_HeldItem { Item::GoldSword };

	float m_Speed {};
	float m_JumpStrength {};
	bool m_Moving {};
	bool m_Falling {};
};
