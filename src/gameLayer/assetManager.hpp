#pragma once

#include <filesystem>
#include <unordered_map>

#include "items.hpp"
#include "raylib.h"

struct AssetManager {
	Texture2D textures {};
	Texture2D frame {};
	Texture2D treeTextures {};
	Texture2D player {};
	Texture2D slime {};
	Texture2D desertSlime {};
	Texture2D iceSlime {};
	Texture2D zombie {};
	Texture2D items {};
	Texture2D hearts {};

	std::unordered_map<int, Texture2D> frontArmor {};
	std::unordered_map<int, Texture2D> backArmor {};
	std::unordered_map<int, Texture2D> feetArmor {};
	std::unordered_map<int, Texture2D> headArmor {};

	// backgrounds
	Texture2D forestMountainsClose {};
	Texture2D forestMountainsFar {};
	Texture2D forestTrees {};
	Texture2D desertDunesBack {};
	Texture2D desertDunesFront {};
	Texture2D snowMountainsBack {};
	Texture2D snowMountainsFront {};
	Texture2D snowTrees {};
	Texture2D snowHill {};
	Texture2D caveBg {};
	Texture2D clouds {};
	Texture2D cloudsNight {};
	Texture2D moon {};
	Texture2D moonStandalone {};
	Texture2D nightSky {};
	Texture2D sky {};
	Texture2D stars {};
	Texture2D sun {};
	Texture2D sunStandalone {};

	void loadAll(const char* texturePackPath = nullptr);

	Texture2D getHeadTexture(Item::Type item);
	Texture2D getBackTexture(Item::Type item);
	Texture2D getFeetTexture(Item::Type item);
	Texture2D getFrontTexture(Item::Type item);

private:
	void unloadTexture(Texture2D& texture);
	Texture2D loadTexture(const std::filesystem::path& defaultPath,
	    const std::filesystem::path* texturePackPath);
};
