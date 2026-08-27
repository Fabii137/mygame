#include "player.hpp"

#include "asserts.h"
#include "assetManager.hpp"
#include "helpers.hpp"
#include "items.hpp"
#include "raylib.h"

#include "nlohmann/json.hpp"

Player::Player()
    : m_Speed(DEFAULT_SPEED)
    , m_JumpStrength(DEFAULT_JUMP_STRENGTH) {
	setColliderSize();
	m_Health = maxHealth();
}

void Player::render(AssetManager& assetManager) const {
	Rectangle aabb { m_Physics.transform.getAABB() };

	Color tint { isHit() > 0.f ? RED : WHITE };
	int animPosY { m_HeldItem ? m_Animation.positionY + 3
		                        : m_Animation.positionY };
	Rectangle textureUV { getTextureAtlas(
		  m_Animation.positionX, animPosY, 32, 64, m_Animation.movingLeft) };

	drawTexture(assetManager.getBackTexture(m_ArmorChest), textureUV, aabb, tint);
	drawTexture(assetManager.getFeetTexture(m_ArmorLegs), textureUV, aabb, tint);
	drawTexture(assetManager.getHeadTexture(m_ArmorHead), textureUV, aabb, tint);
	renderHeldItem(assetManager, aabb);
	drawTexture(
	    assetManager.getFrontTexture(m_ArmorChest), textureUV, aabb, tint);
	DrawRectangleLinesEx(aabb, 0.1f, { 20, 101, 250, 120 });
}

bool Player::update(float dt, EntityUpdateData& updateData) {
	updateTimers(dt);
	m_Moving = false;
	m_Falling = false;

	if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
		position().x -= m_Speed * dt;
		m_Moving = true;
		m_Animation.movingLeft = true;
	}
	if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
		position().x += m_Speed * dt;
		m_Moving = true;
		m_Animation.movingLeft = false;
	}
	if (updateData.creativeMode) {
		if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
			position().y -= m_Speed * dt;
			m_Moving = true;
		}
		if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
			position().y += m_Speed * dt;
			m_Moving = true;
		}
	}
	if (IsKeyDown(KEY_SPACE)) {
		physics().jump(m_JumpStrength);
	}

	if (m_Physics.downTouch) {
		m_Falling = false;
	} else {
		m_Falling = true;
	}

	if (m_Falling) {
		m_Animation.setAnimation(2);
	} else if (m_Moving) {
		m_Animation.setAnimation(1);
	} else {
		m_Animation.setAnimation(0);
	}

	m_Animation.update(dt, 0.08, 7);

	return true;
}

EntityType Player::type() const { return EntityType::Player; }

float Player::maxHealth() const { return 10.f; }

void Player::setColliderSize() {
	m_Physics.transform.w = 0.8f;
	m_Physics.transform.h = 1.6f;
}

float& Player::speed() { return m_Speed; }

float Player::speed() const { return m_Speed; };

void Player::renderHeldItem(AssetManager& assetManager, Rectangle aabb) const {
	if (!m_HeldItem) {
		return;
	}

	Texture2D texture { getTextureForItemType(m_HeldItem, assetManager) };
	Rectangle textureUVItem { getTextureCoordsForItemType(m_HeldItem) };
	if (isBlock(m_HeldItem) || isWall(m_HeldItem)) {
		aabb.width = 0.4f;
		aabb.height = 0.4f;
		if (m_Animation.movingLeft) {
			aabb.y += 0.5f;
			aabb.x -= 0.2f;
		} else {
			aabb.y += 0.5f;
			aabb.x += 0.6f;
		}
	} else if (isItem(m_HeldItem)) {
		aabb.width = 1.f;
		aabb.height = 1.f;
		if (m_Animation.movingLeft) {
			aabb.y += 0.1f;
			aabb.x -= 0.7f;
			textureUVItem = flipTextureAtlasX(textureUVItem);
		} else {
			aabb.y += 0.1f;
			aabb.x += 0.5f;
		}
	} else {
		permaAssertCommentDevelopment(false, "Unknown held item type");
	}
	drawTexture(texture, textureUVItem, aabb);
}

float& Player::jumpStrength() { return m_JumpStrength; }

float Player::jumpStrength() const { return m_JumpStrength; }

Json Player::formatToJson() const {
	Json json {};
	addEntityCommonToJson(json);

	json["speed"] = m_Speed;
	json["jumpStrength"] = m_JumpStrength;

	return json;
}

bool Player::loadFromJson(Json& json) {
	*this = {};

	if (!loadEntityCommonFromJson(json)) {
		return false;
	}

	if (json.contains("speed") && json["speed"].is_number()) {
		m_Speed = json["speed"];
	}

	if (json.contains("jumpStrength") && json["jumpStrength"].is_number()) {
		m_JumpStrength = json["jumpStrength"];
	}

	setColliderSize();

	return true;
}
