#include <cmath>
#include <cstdlib>

#include <algorithm>

#include "physics.hpp"

#include "gameMap.hpp"
#include "raymath.h"

Vector2 Transform2D::getCenter() const { return { pos.x, pos.y }; }

Vector2 Transform2D::getTop() const { return { pos.x, pos.y - h * 0.5f }; }

Vector2 Transform2D::getBottom() const { return { pos.x, pos.y + h * 0.5f }; }

Vector2 Transform2D::getLeft() const { return { pos.x - w * 0.5f, pos.y }; }

Vector2 Transform2D::getRight() const { return { pos.x + w * 0.5f, pos.y }; }

Vector2 Transform2D::getTopLeft() const {
	return { pos.x - w * 0.5f, pos.y - h * 0.5f };
}

Vector2 Transform2D::getTopRight() const {
	return { pos.x + w * 0.5f, pos.y - h * 0.5f };
}

Vector2 Transform2D::getBottomLeft() const {
	return { pos.x - w * 0.5f, pos.y + h * 0.5f };
}

Vector2 Transform2D::getBottomRight() const {
	return { pos.x + w * 0.5f, pos.y + h * 0.5f };
}

Rectangle Transform2D::getAABB() const {
	return { pos.x - w * 0.5f, pos.y - h * 0.5f, w, h };
}

bool Transform2D::intersectPoint(Vector2 point, float delta) const {
	Rectangle aabb { getAABB() };
	aabb.x -= delta;
	aabb.y -= delta;
	aabb.width += 2 * delta;
	aabb.height += 2 * delta;

	return CheckCollisionPointRec(point, aabb);
}

bool Transform2D::intersectTransform(Transform2D other, float delta) const {
	Rectangle a { getAABB() };
	Rectangle b { other.getAABB() };

	a.x -= delta;
	a.y -= delta;
	a.width += 2 * delta;
	a.height += 2 * delta;

	b.x -= delta;
	b.y -= delta;
	b.width += 2 * delta;
	b.height += 2 * delta;

	return CheckCollisionRecs(a, b);
}

void PhysicalEntity::teleport(Vector2 pos) {
	transform.pos = pos;
	lastPosition = pos;
}

void PhysicalEntity::updateForces(float deltaTime) {
	velocity += acceleration * deltaTime;
	transform.pos += velocity * deltaTime;

	Vector2 dragVector { velocity.x * std::abs(velocity.x),
		velocity.y * std::abs(velocity.y) };
	constexpr float drag { 0.01f };

	if (Vector2Length(dragVector) * drag * deltaTime > Vector2Length(velocity)) {
		velocity = {};
	} else {
		velocity -= dragVector * drag * deltaTime;
	}

	if (Vector2Length(velocity) < 0.01f) {
		velocity = {};
	}

	acceleration = {};
}

void PhysicalEntity::updateFinal() { lastPosition = transform.pos; }

void PhysicalEntity::applyGravity() { acceleration += { 0.f, 20.f }; }

void PhysicalEntity::jump(float force) {
	if (downTouch) {
		velocity.y = -force;
	}
}

void PhysicalEntity::resolveConstraints(GameMap& mapData) {
	upTouch = false;
	downTouch = false;
	leftTouch = false;
	rightTouch = false;

	Vector2& pos { transform.pos };
	float distance = Vector2Distance(lastPosition, pos);
	if (distance == 0.f) {
		return;
	}

	constexpr float granularity { 0.8f };
	if (distance <= granularity) {
		checkCollisionOnce(pos, mapData);
	} else {
		Vector2 newPos = lastPosition;
		Vector2 delta = pos - lastPosition;
		delta = Vector2Normalize(delta);
		delta *= granularity * 0.99f;

		bool collision {};
		do {
			newPos += delta;
			Vector2 posTest = newPos;
			checkCollisionOnce(newPos, mapData);

			if (newPos != posTest) {
				pos = newPos;
				collision = true;
				break;
			}
		} while (Vector2Length((newPos + delta) - pos) > granularity);

		if (!collision) {
			checkCollisionOnce(pos, mapData);
		}
	}

	if (pos.x - transform.w / 2.f < 0) {
		pos.x = transform.w / 2.f;
	}
	if (pos.x + transform.w / 2.f > mapData.w) {
		pos.x = mapData.w - transform.w / 2.f;
	}
	if (pos.y + transform.h / 2.f > mapData.h) {
		pos.y = mapData.h - transform.h / 2.f;
	}

	if (leftTouch && velocity.x < 0) {
		velocity.x = 0.f;
	}
	if (rightTouch && velocity.x > 0) {
		velocity.x = 0.f;
	}
	if (upTouch && velocity.y < 0) {
		velocity.y = 0.f;
	}
	if (downTouch && velocity.y > 0) {
		velocity.y = 0.f;
	}
}

void PhysicalEntity::checkCollisionOnce(Vector2& pos, GameMap& mapData) {
	Vector2 delta { pos - lastPosition };
	Vector2 newPos { performCollisionOnOneAxis(
		  mapData, { pos.x, lastPosition.y }, { delta.x, 0.f }) };

	pos = performCollisionOnOneAxis(
	    mapData, { newPos.x, pos.y }, { 0.f, delta.y });
}

Vector2 PhysicalEntity::performCollisionOnOneAxis(
    GameMap& mapData, Vector2 pos, Vector2 delta) {
	if (delta.x == 0 && delta.y == 0) {
		return pos;
	}

	Vector2 dimensions { transform.w, transform.h };
	Vector2 halfDimensions { dimensions / 2.f };
	int minX { static_cast<int>(std::floor(pos.x - dimensions.x / 2.f - 1.f)) };
	int maxX { static_cast<int>(std::ceil(pos.x + dimensions.x / 2.f + 1.f)) };
	int minY { static_cast<int>(std::floor(pos.y - dimensions.y / 2.f - 1.f)) };
	int maxY { static_cast<int>(std::ceil(pos.y + dimensions.y / 2.f + 1.f)) };

	minX = std::max(minX, 0);
	minY = std::max(minY, 0);
	maxX = std::min(maxX, mapData.w);
	maxY = std::min(maxY, mapData.h);

	for (int y { minY }; y < maxY; y++) {
		for (int x { minX }; x < maxX; x++) {
			if (mapData.getBlockUnsafe(x, y).isCollidable()) {
				Transform2D entity {
					.pos = pos,
					.w = dimensions.x,
					.h = dimensions.y,
				};

				Transform2D block {
					.pos = { x + 0.5f, y + 0.5f },
					.w = 1.f,
					.h = 1.f,
				};

				if (entity.intersectTransform(block, -0.00005f)) {
					if (delta.x != 0.f) {
						if (delta.x < 0.f) { // moving left
							leftTouch = true;
							pos.x = x + 1.f + dimensions.x / 2.f;
						} else {
							rightTouch = true;
							pos.x = x - dimensions.x / 2.f;
						}
					} else {
						if (delta.y < 0.f) { // moving up
							upTouch = true;
							pos.y = y + 1.f + dimensions.y / 2.f;
						} else {
							downTouch = true;
							pos.y = y - dimensions.y / 2.f;
						}
					}
					return pos;
				}
			}
		}
	}

	return pos;
}
