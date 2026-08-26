#pragma once

#include "raylib.h"

struct Transform2D {
	Vector2 pos {};
	float w {};
	float h {};

	Vector2 center() const;
	Vector2 top() const;
	Vector2 bottom() const;
	Vector2 left() const;
	Vector2 right() const;
	Vector2 topLeft() const;
	Vector2 topRight() const;
	Vector2 bottomLeft() const;
	Vector2 bottomRight() const;

	Rectangle getAABB() const;

	bool intersectPoint(Vector2 point, float delta = 0.f) const;

	bool intersectTransform(Transform2D other, float delta = 0.f) const;
};

struct GameMap;

struct PhysicalEntity {
	Transform2D transform {};
	Vector2 lastPosition {};

	Vector2 velocity {};
	Vector2 acceleration {};

	bool upTouch {};
	bool downTouch {};
	bool leftTouch {};
	bool rightTouch {};

	void teleport(Vector2 pos);
	void updateForces(float deltaTime);
	void updateFinal();
	void applyGravity();

	void jump(float force);

	void resolveConstraints(GameMap& mapData);
	void checkCollisionOnce(Vector2& pos, GameMap& mapData);
	Vector2 performCollisionOnOneAxis(
	    GameMap& mapData, Vector2 pos, Vector2 delta);
};
