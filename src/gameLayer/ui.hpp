#pragma once

#include "raylib.h"

namespace UI {

Rectangle placeRectTopRight(Rectangle rect, float screenWidth);

Rectangle placeRectTopLeft(Rectangle rect);

Rectangle placeRectBottomRight(Rectangle rect, Vector2 screenSize);

Rectangle placeRectBottomLeft(Rectangle rect, float screenHeight);

Rectangle placeRectCenter(Rectangle rect, Vector2 screenSize);

Rectangle placeRectCenterTop(Rectangle rect, float screenWidth);

Rectangle placeRectCenterBottom(Rectangle rect, Vector2 screenSize);

Rectangle placeRectCenterLeft(Rectangle rect, float screenHeight);

Rectangle placeRectCenterRight(Rectangle rect, Vector2 screenSize);

Rectangle enlargeRectPixels(Rectangle rect, float pixelsX, float pixelsY);

Rectangle enlargeRectPercentage(
    Rectangle rect, float percentageX, float percentageY);

Rectangle shrinkRectPercentage(
    Rectangle rect, float percentageX, float percentageY);

Rectangle shrinkRectPixels(Rectangle rect, float pixelsX, float pixelsY);

void splitRectVertically(
    Rectangle rect, Rectangle& left, Rectangle& right, float percentageLeft);

void splitRectHorizontally(
    Rectangle rect, Rectangle& top, Rectangle& bottom, float percentageTop);

}
