#pragma once

namespace Audio {
void init();
void loadAllMusicAndSounds();
void update();
void playSound(int sound, float volume = 1.f);
void playMusic(int music);
void stopMusic();
bool isMusicPlaying();

enum Sounds {
  NoneSound = 0,
  PlaceBlock,
  BreakBlock,
  FinalHit,
  Hit,

  SOUNDS_COUNT,
};

enum Musics {
  NoneMusic = 0,
  MusicForest,
  MusicDesert,
  MusicSnow,
  MusicCave,

  MUSIC_COUNT,
};
} // namespace Audio
