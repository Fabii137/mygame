#pragma once

struct EntityAnimation {
  int positionX{};
  int positionY{};

  float timer{};

  void update(float dt, float frameDuration, int framesCount) {
    timer -= dt;

    if (timer <= 0.f) {
      timer += frameDuration;
      positionX++;
    }

    positionX %= framesCount;
  }

  void setAnimation(int animation) {
    if (positionY != animation) {
      positionX = 0;
      positionY = animation;
    }
  }
};
