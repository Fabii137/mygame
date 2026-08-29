#pragma once

#include "entity.hpp"
#include "entityAnimation.hpp"
#include "items.hpp"

class Player : public Entity {
public:
	Player();

	void render(AssetManager& assetManager) const override;
	bool update(float dt, EntityUpdateData& updateData) override;

	EntityType type() const override;
	float maxHealth() const override;

	void setColliderSize() override;

	float& speed();
	float speed() const;

	float& jumpStrength();
	float jumpStrength() const;

	Json formatToJson() const override;
	bool loadFromJson(Json& json) override;

private:
	void renderHeldItem(AssetManager& assetManager, Rectangle aabb) const;

private:
	static constexpr float DEFAULT_SPEED { 10.f };
	static constexpr float DEFAULT_JUMP_STRENGTH { 10.f };

	EntityAnimation m_Animation {};

	Item::Type m_ArmorHead { Item::PartyHat };
	Item::Type m_ArmorChest { Item::GoldChestPlate };
	Item::Type m_ArmorLegs { Item::IceBoots };
	Item::Type m_HeldItem {};

	float m_Speed {};
	float m_JumpStrength {};
	bool m_Moving {};
	bool m_Falling {};
};
