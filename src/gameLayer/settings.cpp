#include <filesystem>
#include <fstream>

#include "settings.hpp"

#include "nlohmann/json.hpp"

constexpr const char* SETTINGS_FOLDER_PATH { RESOURCES_PATH "../settings" };
constexpr const char* SETTINGS_FILE_PATH { RESOURCES_PATH
	"../settings/settings.txt" };

Settings settings {};
Settings settingsOld {};

Settings& getSettings() { return settings; }

using Json = nlohmann::json;

void saveSettings() {
	Json json {};

	json["masterVolume"] = settings.masterVolume;
	json["musicVolume"] = settings.musicVolume;
	json["soundsVolume"] = settings.soundsVolume;

	std::error_code error {};
	std::filesystem::create_directory(SETTINGS_FOLDER_PATH, error);

	std::ofstream f { SETTINGS_FILE_PATH };
	f << json.dump(2);
}

void loadSettings() {
	settings = {};

	std::ifstream f { SETTINGS_FILE_PATH };
	if (!f.is_open()) {
		return;
	}

	Json json = Json::parse(f, nullptr, false);
	if (json["masterVolume"].is_number()) {
		settings.masterVolume = json["masterVolume"].get<float>();
	}
	if (json["musicVolume"].is_number()) {
		settings.musicVolume = json["musicVolume"].get<float>();
	}
	if (json["soundsVolume"].is_number()) {
		settings.soundsVolume = json["soundsVolume"].get<float>();
	}

	settings.sanitize();
}

void updateSettings() {
	if (settings != settingsOld) {
		saveSettings();
	}

	settingsOld = settings;
}
