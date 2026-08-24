#pragma once

namespace Audio {

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

void init();
void loadAllMusicAndSounds();
void update(float dt);
void playSound(int sound, float volume = 1.f);
void playMusic(Musics music);
void stopMusic();
bool isMusicPlaying();

} // namespace Audio
