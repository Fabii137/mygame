#include <algorithm>

#include "entity.hpp"

#include "nlohmann/json.hpp"

Vector2& Entity::position() { return m_Physics.transform.pos; }

const Vector2& Entity::position() const { return m_Physics.transform.pos; }

float Entity::health() const { return m_Health; }

PhysicalEntity& Entity::physics() { return m_Physics; }

float& Entity::timeOutsideDespawnRange() { return m_TimeOutsideDespawnRange; }

void Entity::teleport(Vector2 pos) { m_Physics.teleport(pos); }

void Entity::takeDamage(float damage) {
	if (m_HitTimer > 0.f) {
		return;
	}

	m_Health = std::max(m_Health - damage, 0.f);
	m_RedTimer = RED_TIMER_DURATION;
	m_HitTimer = HIT_TIMER_DURATION;
}

void Entity::heal(float amount) {
	m_Health = std::clamp(m_Health + amount, 0.f, maxHealth());
}

void Entity::updatePhysics(float dt, GameMap& gameMap, bool applyGravity) {
	if (applyGravity) {
		m_Physics.applyGravity();
	}
	m_Physics.updateForces(dt);
	m_Physics.resolveConstraints(gameMap);
	m_Physics.updateFinal();
}

bool Entity::isEnemy() const { return false; }

bool Entity::alive() const { return m_Health > 0.f; }

void Entity::kill() { m_Health = 0.f; }

void Entity::updateTimers(float dt) {
	m_RedTimer = std::max(m_RedTimer - dt, 0.f);
	m_HitTimer = std::max(m_HitTimer - dt, 0.f);
}

void Entity::addEntityCommonToJson(Json& json) const {
	json["physics"] = m_Physics.formatToJson();
	json["health"] = m_Health;
	json["entityType"] = type();
}

bool Entity::loadEntityCommonFromJson(Json& json) {
	if (!json.contains("physics") || !json["physics"].is_object()) {
		return false;
	}

	auto physics = json["physics"];
	if (!m_Physics.loadFromJson(physics)) {
		return false;
	}

	if (json.contains("health") && json["health"].is_number()) {
		m_Health = json["health"];
	}

	return true;
}

bool Entity::isHit() const { return m_RedTimer > 0.f; }
