#pragma once

#include "entity.hpp"
#include "entityAnimation.hpp"

enum class SlimeType { Green, Desert, Ice };

class Slime : public Entity {
public:
	Slime(SlimeType type);

	void render(AssetManager& assetManager) const override;
	bool update(float dt, EntityUpdateData& updateData) override;
	EntityType getType() const override;
	float getMaxHealth() const override;
	bool isEnemy() const override;

private:
	enum class State { Wandering, Chasing };

	EntityAnimation m_Animation {};
	State m_CurrentState { State::Wandering };
	SlimeType m_Type {};
	float m_ChangeStateTimer { 1.f };
	float m_JumpTimer { 5.f };
	float m_MoveSpeed { 0.f };
};
