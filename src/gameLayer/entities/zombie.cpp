#include "zombie.hpp"

#include "assetManager.hpp"
#include "constants.hpp"
#include "helpers.hpp"
#include "random.h"
#include "raymath.h"

Zombie::Zombie() {
	m_Physics.transform.w = 0.8f;
	m_Physics.transform.h = 1.6f;
	m_Health = maxHealth();
}

void Zombie::render(AssetManager& assetManager) const {
	Rectangle aabb { getRectangleForEntity(m_Physics.transform, 0.8f, 1.6f) };

	Color tint { isHit() ? ENTITY_HIT_TINT : WHITE };

	drawTextureAtlas(assetManager.zombie, m_Animation.positionX,
	    m_Animation.positionY, aabb, tint, Constants::CELL_SIZE,
	    Constants::CELL_SIZE * 2);
}

bool Zombie::update(float dt, EntityUpdateData& updateData) {
	updateTimers(dt);
	m_ChangeStateTimer -= dt;

	if (m_ChangeStateTimer < 0.f) {
		m_ChangeStateTimer = getRandomFloat(updateData.rng, 1, 7);

		float distanceToPlayer { Vector2Distance(
			  updateData.playerPosition, position()) };
		if (distanceToPlayer < 2.f) {
			m_CurrentState = State::Attack;
		} else if (distanceToPlayer < 15.f) {
			m_CurrentState = State::Chasing;
		} else {
			if (getRandomChance(updateData.rng, 0.5f)) {
				m_CurrentState = State::Idle;
			} else {
				m_CurrentState = State::Wandering;
				if (getRandomChance(updateData.rng, 0.5f)) {
					m_WanderingDir = 1.f;
				} else {
					m_WanderingDir = -1.f;
				}
			}
		}
	}

	switch (m_CurrentState) {
	case State::Idle: {
		m_Animation.setAnimation(0);
		m_MoveSpeed = 0.f;
	} break;
	case State::Wandering: {
		m_Animation.setAnimation(1);
		m_MoveSpeed = m_WanderingDir * 3.f;
	} break;
	case State::Chasing: {
		m_Animation.setAnimation(1);
		float speed { getRandomFloat(updateData.rng, 5, 5) };
		if (updateData.playerPosition.x > position().x) {
			m_MoveSpeed = speed;
		} else {
			m_MoveSpeed = -speed;
		}
	} break;
	case State::Attack: {
		// TODO
	} break;
	}

	if (m_MoveSpeed) {
		position().x += m_MoveSpeed * dt;
	}

	m_Animation.update(dt, 0.08f, 7);

	return true;
}

EntityType Zombie::type() const { return EntityType::Zombie; }

float Zombie::maxHealth() const { return 20.f; }

bool Zombie::isEnemy() const { return true; }
