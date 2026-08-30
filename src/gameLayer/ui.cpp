#include "ui.hpp"

namespace UI {

Rectangle placeRectTopRight(Rectangle rect, float screenWidth) {
	rect.x = screenWidth - rect.width;
	rect.y = 0;
	return rect;
}

Rectangle placeRectCenterBottom(Rectangle rect, Vector2 screenSize) {
	rect.x = (screenSize.x - rect.width) / 2.f;
	rect.y = screenSize.y - rect.width;
	return rect;
}

Rectangle placeRectTopLeft(Rectangle rect) {
	rect.x = 0;
	rect.y = 0;
	return rect;
}

Rectangle placeRectBottomRight(Rectangle rect, Vector2 screenSize) {
	rect.x = screenSize.x - rect.width;
	rect.y = screenSize.y - rect.height;
	return rect;
}

Rectangle placeRectBottomLeft(Rectangle rect, float screenHeight) {
	rect.x = 0;
	rect.y = screenHeight - rect.height;
	return rect;
}

Rectangle placeRectCenter(Rectangle rect, Vector2 screenSize) {
	rect.x = (screenSize.x - rect.width) / 2.f;
	rect.y = (screenSize.y - rect.height) / 2.f;
	return rect;
}

Rectangle placeRectCenterTop(Rectangle rect, float screenWidth) {
	rect.x = (screenWidth - rect.width) / 2.f;
	rect.y = 0;
	return rect;
}

Rectangle placeRectCenterLeft(Rectangle rect, float screenHeight) {
	rect.x = 0;
	rect.y = (screenHeight - rect.height) / 2.f;
	return rect;
}

Rectangle placeRectCenterRight(Rectangle rect, Vector2 screenSize) {
	rect.x = screenSize.x - rect.width;
	rect.y = (screenSize.y - rect.height) / 2.f;
	return rect;
}

Rectangle shrinkRectPixels(Rectangle rect, float pixelsX, float pixelsY) {
	rect.width -= pixelsX;
	rect.height -= pixelsY;

	rect.x += pixelsX / 2.f;
	rect.y += pixelsY / 2.f;

	return rect;
}

Rectangle shrinkRectPercentage(
    Rectangle rect, float percentageX, float percentageY) {
	float shrinkX { rect.width * percentageX };
	float shrinkY { rect.height * percentageY };

	rect.width -= shrinkX;
	rect.height -= shrinkY;

	rect.x += shrinkX / 2.f;
	rect.y += shrinkY / 2.f;

	return rect;
}

Rectangle enlargeRectPixels(Rectangle rect, float pixelsX, float pixelsY) {
	rect.width += pixelsX;
	rect.height += pixelsY;

	rect.x -= pixelsX / 2.f;
	rect.y -= pixelsY / 2.f;

	return rect;
}

Rectangle enlargeRectPercentage(
    Rectangle rect, float percentageX, float percentageY) {
	float enlargeX { rect.width * percentageX };
	float enlargeY { rect.height * percentageY };

	rect.width += enlargeX;
	rect.height += enlargeY;

	rect.x -= enlargeX / 2.f;
	rect.y -= enlargeY / 2.f;

	return rect;
}

void splitRectVertically(
    Rectangle rect, Rectangle& left, Rectangle& right, float percentageLeft) {
	left.width = rect.width * percentageLeft;
	left.height = rect.height;
	left.x = rect.x;
	left.y = rect.y;

	right.width = rect.width - left.width;
	right.height = rect.height;
	right.x = rect.x + left.width;
	right.y = rect.y;
}

void splitRectHorizontally(
    Rectangle rect, Rectangle& top, Rectangle& bottom, float percentageTop) {
	top.width = rect.width;
	top.height = rect.height * percentageTop;
	top.x = rect.x;
	top.y = rect.y;

	bottom.width = rect.width;
	bottom.height = rect.height - top.height;
	bottom.x = rect.x;
	bottom.y = rect.y + top.height;
}

}
