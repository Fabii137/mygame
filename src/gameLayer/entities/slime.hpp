#pragma once

#include "entity.hpp"
#include "entityAnimation.hpp"

enum class SlimeType { Green, Desert, Ice, SLIME_TYPE_COUNT };

class Slime : public Entity {
public:
	Slime();

	void render(AssetManager& assetManager) const override;
	bool update(float dt, EntityUpdateData& updateData) override;
	EntityType type() const override;
	float maxHealth() const override;
	bool isEnemy() const override;

	SlimeType& slimeType();

	void setColliderSize() override;

	Json formatToJson() const override;
	bool loadFromJson(Json& json) override;

private:
	enum class State { Wandering, Chasing };

	EntityAnimation m_Animation {};
	State m_CurrentState { State::Wandering };
	SlimeType m_SlimeType {};
	float m_ChangeStateTimer { 1.f };
	float m_JumpTimer { 5.f };
	float m_MoveSpeed { 0.f };
};
