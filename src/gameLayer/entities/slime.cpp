#include "slime.hpp"

#include "asserts.h"
#include "assetManager.hpp"
#include "entity.hpp"
#include "helpers.hpp"
#include "random.h"
#include "raymath.h"

#include "nlohmann/json.hpp"

Slime::Slime() {
	setColliderSize();
	m_Health = maxHealth();
}

void Slime::render(AssetManager& assetManager) const {
	Rectangle aabb { getRectangleForEntity(m_Physics.transform, 1, 1) };

	Color tint { isHit() ? ENTITY_HIT_TINT : WHITE };

	switch (m_SlimeType) {
	case SlimeType::Green:
		drawTextureAtlas(assetManager.slime, m_Animation.positionX,
		    m_Animation.positionY, aabb, tint);
		break;
	case SlimeType::Desert:
		drawTextureAtlas(assetManager.desertSlime, m_Animation.positionX,
		    m_Animation.positionY, aabb, tint);
		break;
	case SlimeType::Ice:
		drawTextureAtlas(assetManager.iceSlime, m_Animation.positionX,
		    m_Animation.positionY, aabb, tint);
		break;
	case SlimeType::SLIME_TYPE_COUNT:
		permaAssertCommentDevelopment(false, "Invalid SlimeType");
		break;
	}
}

bool Slime::update(float dt, EntityUpdateData& updateData) {
	updateTimers(dt);
	m_ChangeStateTimer -= dt;

	if (m_ChangeStateTimer < 0.f) {
		m_ChangeStateTimer = getRandomFloat(updateData.rng, 1, 7);

		float distanceToPlayer { Vector2Distance(
			  updateData.playerPosition, position()) };
		if (distanceToPlayer < 15.f) {
			m_CurrentState = State::Chasing;
		} else {
			m_CurrentState = State::Wandering;
		}
	}

	if (m_Physics.downTouch) {
		m_MoveSpeed = 0.f;
		m_Animation.setAnimation(0);
	} else {
		m_Animation.setAnimation(1);
	}

	m_JumpTimer -= dt;

	switch (m_CurrentState) {
	case State::Wandering: {
		if (m_JumpTimer < 0.f) {
			m_JumpTimer = getRandomFloat(updateData.rng, 3, 12);
			m_Physics.jump(10);
			m_MoveSpeed = getRandomFloat(updateData.rng, -7, 7);
		}
	} break;
	case State::Chasing: {
		if (m_JumpTimer < 0.f) {
			m_JumpTimer = getRandomFloat(updateData.rng, 2, 7);
			m_Physics.jump(10);
			float speed { getRandomFloat(updateData.rng, 1, 7) };
			if (updateData.playerPosition.x > position().x) {
				m_MoveSpeed = speed;
			} else {
				m_MoveSpeed = -speed;
			}
		}
	} break;
	}

	if (m_MoveSpeed) {
		position().x += m_MoveSpeed * dt;
	}

	m_Animation.update(dt, 0.08f, 7);

	return true;
}

EntityType Slime::type() const { return EntityType::Slime; }

float Slime::maxHealth() const {
	switch (m_SlimeType) {
	case SlimeType::Green:
		return 10.f;
	case SlimeType::Ice:
		return 20.f;
	case SlimeType::Desert:
		return 30.f;
	case SlimeType::SLIME_TYPE_COUNT:
		break;
	}

	permaAssertCommentDevelopment(false, "Invalid SlimeType");
	return 0.f;
}

bool Slime::isEnemy() const { return true; }

SlimeType& Slime::slimeType() { return m_SlimeType; }

void Slime::setColliderSize() {
	m_Physics.transform.w = 0.8f;
	m_Physics.transform.h = 0.8f;
}

Json Slime::formatToJson() const {
	Json json {};
	addEntityCommonToJson(json);

	json["slimeType"] = m_SlimeType;

	return json;
}

bool Slime::loadFromJson(Json& json) {
	*this = {};

	if (!loadEntityCommonFromJson(json)) {
		return false;
	}

	if (json.contains("slimeType") && json["slimeType"].is_number()) {
		m_SlimeType = json["slimeType"];
		if (static_cast<int>(m_SlimeType) < 0
		    || m_SlimeType >= SlimeType::SLIME_TYPE_COUNT) {
			return false;
		}
	}

	return true;
}
