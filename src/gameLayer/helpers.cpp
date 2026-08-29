#include "helpers.hpp"

#include "physics.hpp"
#include "raylib.h"

Vector2 getScreenSize() {
	return {
		static_cast<float>(GetScreenWidth()),
		static_cast<float>(GetScreenHeight()),
	};
}

Rectangle getTextureAtlas(
    int x, int y, int cellSizeX, int cellSizeY, bool flipX) {
	float sizeX { static_cast<float>(cellSizeX) };
	if (flipX) {
		sizeX *= -1.f;
	}
	return shrinkUV({
	    static_cast<float>(x) * cellSizeX,
	    static_cast<float>(y) * cellSizeY,
	    sizeX,
	    static_cast<float>(cellSizeY),
	});
}

Rectangle flipTextureAtlasX(Rectangle rect) {
	rect.width *= -1.f;
	return rect;
}

Rectangle shrinkUV(Rectangle rect, float shrink) {
	rect.width -= shrink;
	rect.height -= shrink;
	rect.x += shrink * 0.5f;
	rect.y += shrink * 0.5f;
	return rect;
}

Rectangle getRectangleForEntity(
    Transform2D transform, float textureW, float textureH) {
	Transform2D result { transform };
	result.w = textureW;
	result.h = textureH;

	result.pos.y -= (result.h - transform.h) / 2.f;

	return result.getAABB();
}

void drawTextureAtlas(const Texture2D& texture, int atlasX, int atlasY,
    Rectangle dest, Color tint, int cellSizeX, int cellSizeY) {
	DrawTexturePro(texture, getTextureAtlas(atlasX, atlasY, cellSizeX, cellSizeY),
	    dest, {}, 0.f, tint);
}

void drawTexture(
    const Texture2D& texture, Rectangle src, Rectangle dest, Color tint) {
	DrawTexturePro(texture, src, dest, {}, 0.f, tint);
}
