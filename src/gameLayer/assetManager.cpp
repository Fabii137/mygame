#include <filesystem>
#include <string>

#include "assetManager.hpp"

#include "items.hpp"
#include "raylib.h"

namespace fs = std::filesystem;

void AssetManager::loadAll(const char* texturePackPath) {
	auto load = [&](Texture2D& texture, const std::string& file) {
		fs::path defaultPath { fs::path(RESOURCES_PATH) / file };

		unloadTexture(texture);
		if (texturePackPath) {
			fs::path packPath { fs::path(texturePackPath) / file };
			texture = loadTexture(defaultPath, &packPath);
			return;
		}

		texture = loadTexture(defaultPath, nullptr);
	};

	load(textures, "texturesWithBackgroundVersion.png");
	load(frame, "frame.png");
	load(treeTextures, "treetextures.png");
	load(player, "player.png");
	load(slime, "slime.png");
	load(desertSlime, "desertslime.png");
	load(iceSlime, "iceslime.png");
	load(zombie, "zombie.png");
	load(items, "items.png");

	// backgrounds
	load(forestMountainsClose, "backgrounds/forest/mountainsClose.png");
	load(forestMountainsFar, "backgrounds/forest/mountainsFar.png");
	load(forestTrees, "backgrounds/forest/trees.png");
	load(desertDunesBack, "backgrounds/desert/dunesBack.png");
	load(desertDunesFront, "backgrounds/desert/dunesFront.png");
	load(snowMountainsBack, "backgrounds/snow/mountainsBack.png");
	load(snowMountainsFront, "backgrounds/snow/mountainsFront.png");
	load(snowTrees, "backgrounds/snow/trees.png");
	load(snowHill, "backgrounds/snow/hill.png");
	load(caveBg, "backgrounds/caveBG.png");
	load(clouds, "backgrounds/sky/clouds.png");
	load(cloudsNight, "backgrounds/sky/cloudsNight.png");
	load(moon, "backgrounds/sky/moon.png");
	load(moonStandalone, "backgrounds/sky/moonStandalone.png");
	load(nightSky, "backgrounds/sky/nightSky.png");
	load(sky, "backgrounds/sky/sky.png");
	load(stars, "backgrounds/sky/stars.png");
	load(sun, "backgrounds/sky/sun.png");
	load(sunStandalone, "backgrounds/sky/sunStandalone.png");

	// player / armor
	load(feetArmor[0], "body/player_feet.png");
	load(headArmor[0], "body/player_head.png");
	load(frontArmor[0], "body/player_front.png");
	load(backArmor[0], "body/player_back.png");

	load(headArmor[Item::PartyHat], "body/party_hat.png");
	load(headArmor[Item::SunGlasses], "body/sunglasses.png");

	load(feetArmor[Item::CopperBoots], "body/copper_armour_feet.png");
	load(headArmor[Item::CopperHelmet], "body/copper_armour_head.png");
	load(frontArmor[Item::CopperChestPlate], "body/copper_armour_front.png");
	load(backArmor[Item::CopperChestPlate], "body/copper_armour_back.png");

	load(feetArmor[Item::IronBoots], "body/iron_armour_feet.png");
	load(headArmor[Item::IronHelmet], "body/iron_armour_head.png");
	load(frontArmor[Item::IronChestPlate], "body/iron_armour_front.png");
	load(backArmor[Item::IronChestPlate], "body/iron_armour_back.png");

	load(feetArmor[Item::GoldBoots], "body/gold_armour_feet.png");
	load(headArmor[Item::GoldHelmet], "body/gold_armour_head.png");
	load(frontArmor[Item::GoldChestPlate], "body/gold_armour_front.png");
	load(backArmor[Item::GoldChestPlate], "body/gold_armour_back.png");

	load(feetArmor[Item::IceBoots], "body/ice_armour_feet.png");
	load(headArmor[Item::IceHelmet], "body/ice_armour_head.png");
	load(frontArmor[Item::IceChestPlate], "body/ice_armour_front.png");
	load(backArmor[Item::IceChestPlate], "body/ice_armour_back.png");
}

Texture2D AssetManager::getHeadTexture(Item::Type item) {
	auto found { headArmor.find(item) };
	if (found == headArmor.end()) {
		return headArmor[0];
	}
	if (found->second.id == 0) {
		return headArmor[0];
	}

	return found->second;
}

Texture2D AssetManager::getBackTexture(Item::Type item) {
	auto found { backArmor.find(item) };
	if (found == backArmor.end()) {
		return backArmor[0];
	}
	if (found->second.id == 0) {
		return backArmor[0];
	}

	return found->second;
}

Texture2D AssetManager::getFeetTexture(Item::Type item) {
	auto found { feetArmor.find(item) };
	if (found == feetArmor.end()) {
		return feetArmor[0];
	}
	if (found->second.id == 0) {
		return feetArmor[0];
	}

	return found->second;
}

Texture2D AssetManager::getFrontTexture(Item::Type item) {
	auto found { frontArmor.find(item) };
	if (found == frontArmor.end()) {
		return frontArmor[0];
	}
	if (found->second.id == 0) {
		return frontArmor[0];
	}

	return found->second;
}

void AssetManager::unloadTexture(Texture2D& texture) {
	if (IsTextureValid(texture)) {
		UnloadTexture(texture);
	}

	texture = {};
}

Texture2D AssetManager::loadTexture(
    const fs::path& defaultPath, const fs::path* packPath) {
	if (packPath && fs::exists(*packPath)) {
		return LoadTexture(packPath->string().c_str());
	}

	return LoadTexture(defaultPath.string().c_str());
}
