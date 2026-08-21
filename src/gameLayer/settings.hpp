#include <algorithm>

struct Settings {
  float masterVolume{0.5f};
  float musicVolume{0.5f};
  float soundsVolume{0.5f};

  void sanitize() {
    masterVolume = std::clamp(masterVolume, 0.f, 1.f);
    musicVolume = std::clamp(musicVolume, 0.f, 1.f);
    soundsVolume = std::clamp(soundsVolume, 0.f, 1.f);
  }
};

Settings &getSettings();
