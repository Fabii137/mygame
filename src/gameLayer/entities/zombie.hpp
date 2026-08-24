#pragma once

#include "entity.hpp"
#include "entityAnimation.hpp"

class Zombie : public Entity {
public:
	Zombie();

	void render(AssetManager& assetManager) const override;

	bool update(float dt, EntityUpdateData& updateData) override;
	EntityType getType() const override;
	float getMaxHealth() const override;
	bool isEnemy() const override;

private:
	enum class State { Idle, Wandering, Chasing, Attack };

	EntityAnimation m_Animation {};
	State m_CurrentState { State::Wandering };
	float m_WanderingDir { -1.f };
	float m_ChangeStateTimer { 1.f };
	float m_MoveSpeed { 0.f };
};
