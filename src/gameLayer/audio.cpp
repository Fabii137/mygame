#include <cmath>

#include <algorithm>
#include <random>
#include <vector>

#include "audio.hpp"

#include "asserts.h"
#include "random.h"
#include "raylib.h"
#include "settings.hpp"

namespace Audio {
constexpr int MAX_PLAYING_SOUNDS{12};
constexpr float MUSIC_TRANSITION_DURATION{1.f};

int getPlayingSoundsCount();

void init() {
  InitAudioDevice();
  SetMasterVolume(0.9f);

  loadAllMusicAndSounds();
}

std::vector<Music> allMusic{};
Musics currentMusicPlaying{};
Musics currentTransitionMusic{};
float musicTransitionTime{};

std::vector<std::vector<Sound>> allSounds{};
std::ranlux24_base rng{std::random_device{}()};

void loadAllMusicAndSounds() {
  allMusic.push_back({});
  auto loadMusic = [&](const char *path) {
    Music music = LoadMusicStream(path);

    if (music.stream.buffer) {
      allMusic.push_back(music);
    } else {
      allMusic.push_back({});
    }
  };

  loadMusic(RESOURCES_PATH "music/forest.ogg");
  loadMusic(RESOURCES_PATH "music/desert.ogg");
  loadMusic(RESOURCES_PATH "music/snow.ogg");
  loadMusic(RESOURCES_PATH "music/cave.ogg");

  permaAssertComment(allMusic.size() == MUSIC_COUNT, "Forgot to add music!");

  auto loadSound = [&](std::vector<const char *> paths) {
    std::vector<Sound> sounds{};
    sounds.reserve(paths.size());

    for (const char *path : paths) {
      Sound s{LoadSound(path)};

      if (s.stream.buffer) {
        sounds.push_back(s);
      } else {
        sounds.push_back({});
      }
    }
    allSounds.push_back(std::move(sounds));
  };

  allSounds.push_back({});

  loadSound({RESOURCES_PATH "sounds/place.ogg"});
  loadSound({RESOURCES_PATH "sounds/break.ogg"});
  loadSound({RESOURCES_PATH "sounds/bonus/FinalHit1.ogg",
             RESOURCES_PATH "sounds/bonus/FinalHit2.ogg",
             RESOURCES_PATH "sounds/bonus/FinalHit3.ogg"});
  loadSound({RESOURCES_PATH "sounds/bonus/Hit1.ogg",
             RESOURCES_PATH "sounds/bonus/Hit2.ogg",
             RESOURCES_PATH "sounds/bonus/Hit3.ogg"});

  permaAssertComment(allSounds.size() == SOUNDS_COUNT, "Forgot to add sound!");
}

void update(float dt) {
  if (!isMusicPlaying()) {
    currentMusicPlaying = Musics::NoneMusic;
    return;
  }

  float targetVolume{getSettings().masterVolume * getSettings().musicVolume};
  if (currentTransitionMusic != Musics::NoneMusic) {
    musicTransitionTime -= dt;

    float t{
        std::clamp(musicTransitionTime / MUSIC_TRANSITION_DURATION, 0.f, 1.f)};

    SetMusicVolume(allMusic[currentMusicPlaying],
                   std::lerp(0.f, targetVolume, 1.f - t));
    SetMusicVolume(allMusic[currentTransitionMusic],
                   std::lerp(0.f, targetVolume, t));

    UpdateMusicStream(allMusic[currentTransitionMusic]);
    UpdateMusicStream(allMusic[currentMusicPlaying]);

    if (musicTransitionTime <= 0.f) {
      StopMusicStream(allMusic[currentTransitionMusic]);

      currentTransitionMusic = Musics::NoneMusic;
      musicTransitionTime = 0.f;

      SetMusicVolume(allMusic[currentMusicPlaying], targetVolume);
    }

    return;
  }

  SetMusicVolume(allMusic[currentMusicPlaying], targetVolume);
  UpdateMusicStream(allMusic[currentMusicPlaying]);
}

void playSound(int sound, float volume) {
  if (sound <= Sounds::NoneSound || sound >= Sounds::SOUNDS_COUNT) {
    return;
  }

  if (getPlayingSoundsCount() >= MAX_PLAYING_SOUNDS) {
    return;
  }

  volume = std::clamp(volume, 0.f, 1.f);

  volume *= getSettings().masterVolume;
  volume *= getSettings().soundsVolume;

  const auto &variations{allSounds[sound]};
  Sound selectedSound{};

  permaAssertComment(!variations.empty(),
                     "No sounds variations loaded for type");
  if (variations.size() == 1) {
    selectedSound = variations[0];
  } else {
    int idx{getRandomInt(rng, 0, variations.size() - 1)};
    selectedSound = variations[idx];
  }

  SetSoundVolume(selectedSound, volume);
  PlaySound(selectedSound);
}

int getPlayingSoundsCount() {
  int count{};
  for (const auto &soundType : allSounds) {
    for (const Sound &sound : soundType) {
      if (IsSoundPlaying(sound)) {
        count++;
        break;
      }
    }
  }

  return count;
}

void playMusic(Musics music) {
  if (allMusic.size() <= music) {
    return;
  }
  if (currentMusicPlaying == music) {
    return;
  }

  if (currentMusicPlaying != Musics::NoneMusic) {
    currentTransitionMusic = currentMusicPlaying;
  } else {
    currentTransitionMusic = Musics::NoneMusic;
  }

  currentMusicPlaying = music;
  musicTransitionTime = MUSIC_TRANSITION_DURATION;

  allMusic[music].looping = true;
  PlayMusicStream(allMusic[music]);

  // transition starts silent
  SetMusicVolume(allMusic[music], 0.f);
}

void stopMusic() {
  StopMusicStream(allMusic[currentMusicPlaying]);
  currentMusicPlaying = Musics::NoneMusic;
}

bool isMusicPlaying() {
  if (currentMusicPlaying == Musics::NoneMusic) {
    return false;
  }

  return IsAudioStreamPlaying(allMusic[currentMusicPlaying].stream);
}
} // namespace Audio
