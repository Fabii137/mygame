#include <cctype>
#include <cstdint>

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <system_error>
#include <type_traits>

#include "saveMap.hpp"

#include "asserts.h"
#include "blocks.hpp"
#include "entity.hpp"
#include "entityHolder.hpp"
#include "gameMap.hpp"
#include "walls.hpp"

#include "entities/droppedItem.hpp"
#include "entities/player.hpp"
#include "entities/slime.hpp"
#include "entities/zombie.hpp"

#include "nlohmann/json.hpp"

namespace {

using Json = nlohmann::json;

struct BlockSaveRepresentation1 {
	Block::Type type {};

	Block toBlock() const {
		return {
			.type = type,
		};
	}
};

struct WallSaveRepresentation1 {
	Wall::Type type {};

	Wall toWall() const {
		return {
			.type = type,
		};
	}
};

BlockSaveRepresentation1 toBlockRepresentation(Block b) {
	return {
		.type = b.type,
	};
}

WallSaveRepresentation1 toWallRepresentation(Wall w) {
	return {
		.type = w.type,
	};
}

constexpr const char* SAVES_FOLDER_PATH { RESOURCES_PATH "../saves/" };
constexpr const char* MAP_SAVE_PATH { RESOURCES_PATH "../saves/map.bin" };
constexpr const char* MAP_SAVE_PATH_TMP { RESOURCES_PATH
	"../saves/map.bin.tmp" };
constexpr const char* PLAYER_SAVE_PATH { RESOURCES_PATH "../saves/player.txt" };
constexpr const char* PLAYER_SAVE_PATH_TMP { RESOURCES_PATH
	"../saves/player.txt.tmp" };
constexpr const char* BIOMES_SAVE_PATH { RESOURCES_PATH "../saves/biomes.txt" };
constexpr const char* BIOMES_SAVE_PATH_TMP { RESOURCES_PATH
	"../saves/biomes.txt.tmp" };
constexpr const char* ENTITIES_SAVE_PATH { RESOURCES_PATH
	"../saves/entities.txt" };
constexpr const char* ENTITIES_SAVE_PATH_TMP { RESOURCES_PATH
	"../saves/entities.txt.tmp" };

constexpr float ENTITIES_SAVE_DELAY { 5.f };
constexpr float MAP_SAVE_DELAY { 5.f };
constexpr float WORLD_SAVE_DELAY { 60.f };

float entitiesSaveTimer { 0.f };
float mapSaveTimer { 0.f };
float worldSaveTimer { 0.f };

template <typename T>
  requires std::is_base_of_v<Entity, T>
void addEntityFromJson(
    EntityHolder& entities, std::uint64_t id, Json& entityJson) {
	T entity {};
	if (entity.loadFromJson(entityJson)) {
		entities.entities[id] = std::make_unique<T>(entity);
	}
}

std::uint64_t highestEntityId(const EntityHolder& entities) {
	std::uint64_t highest {};
	for (const auto& [id, entity] : entities.entities) {
		highest = std::max(highest, id);
	}
	return highest;
}

void writeMapData(const GameMap& map) {
	saveMapDataToFile(map.mapData, map.wallData, map.w, map.h, MAP_SAVE_PATH_TMP);
}

bool loadMap(GameMap& map) {
	return loadMapDataFromFile(
	    map.mapData, map.wallData, map.w, map.h, MAP_SAVE_PATH);
}

void writePlayer(const Player& player) {
	Json json = player.formatToJson();
	std::ofstream f { PLAYER_SAVE_PATH_TMP };
	f << json.dump(2);
}

bool loadPlayer(Player& player) {
	std::ifstream f { PLAYER_SAVE_PATH };
	if (!f || !f.is_open()) {
		return false;
	}

	Json json = Json::parse(f, nullptr, false);
	if (!json.is_object() || !player.loadFromJson(json)) {
		return false;
	}

	return true;
}

void writeBiomes(const std::vector<Biome>& biomes) {
	Json json {};
	for (const Biome& biome : biomes) {
		json.push_back({
		    { "type", biome.type },
		    { "startX", biome.startX },
		    { "endX", biome.endX },
		});
	}

	std::ofstream f { BIOMES_SAVE_PATH_TMP };
	f << json.dump(2);
}

bool loadBiomes(std::vector<Biome>& biomes, int mapWidth) {
	std::ifstream f { BIOMES_SAVE_PATH };
	if (!f || !f.is_open()) {
		return false;
	}

	Json json = Json::parse(f, nullptr, false);
	if (!json.is_array()) {
		return false;
	}

	for (const Json& biomeJson : json) {
		if (!biomeJson.is_object() || !biomeJson.contains("type")
		    || !biomeJson["type"].is_number() || !biomeJson.contains("startX")
		    || !biomeJson["startX"].is_number() || !biomeJson.contains("endX")
		    || !biomeJson["endX"].is_number()) {
			return false;
		}

		int type { biomeJson["type"] };
		int startX { biomeJson["startX"] };
		int endX { biomeJson["endX"] };
		if (type < 0 || type >= Biome::BIOMES_COUNT || startX < 0 || startX >= endX
		    || endX > mapWidth) {
			return false;
		}

		biomes.push_back({
		    .type = static_cast<Biome::Type>(type),
		    .startX = startX,
		    .endX = endX,
		});
	}

	return true;
}

void writeEntities(const EntityHolder& entities) {
	Json json {};

	for (const auto& [id, entity] : entities.entities) {
		json[std::to_string(id)] = entity->formatToJson();
	}

	std::ofstream f { ENTITIES_SAVE_PATH_TMP };
	f << json.dump(2);
}

void loadEntity(EntityHolder& entities, const std::string& keyStr, Json& json) {
	auto isDigit
	    = [](char c) { return std::isdigit(static_cast<unsigned char>(c)); };

	bool isNumeric { !keyStr.empty() && std::ranges::all_of(keyStr, isDigit) };
	if (!isNumeric) {
		return;
	}

	std::uint64_t id {};
	for (char c : keyStr) {
		id *= 10;
		id += c - '0';
	}

	EntityType type {};
	if (!json.contains("entityType") || !json["entityType"].is_number()) {
		return;
	}

	type = json["entityType"];
	switch (type) {
	case EntityType::Player:
		break; // already handled
	case EntityType::Slime:
		addEntityFromJson<Slime>(entities, id, json);
		break;
	case EntityType::DroppedItem:
		addEntityFromJson<DroppedItem>(entities, id, json);
		break;
	case EntityType::Zombie:
		addEntityFromJson<Zombie>(entities, id, json);
		break;
	}
}

bool loadEntities(EntityHolder& entities) {
	std::ifstream f { ENTITIES_SAVE_PATH };
	if (!f || !f.is_open()) {
		return false;
	}

	Json json = Json::parse(f, nullptr, false);
	if (!json.is_object()) {
		return false;
	}

	for (auto it { json.begin() }; it != json.end(); ++it) {
		loadEntity(entities, it.key(), it.value());
	}

	return true;
}
}

constexpr int VERSION { 1 };

bool saveMapDataToFile(const std::vector<Block>& blocks,
    const std::vector<Wall>& walls, int w, int h, const char* fileName) {

	std::ofstream f { fileName, std::ios::binary };
	if (!f.is_open()) {
		return false;
	}

	size_t totalCount { static_cast<size_t>(w) * static_cast<size_t>(h) };
	permaAssertDevelopment(blocks.size() == totalCount);
	permaAssertDevelopment(!blocks.empty());
	permaAssertDevelopment(walls.size() == totalCount);
	permaAssertDevelopment(!walls.empty());

	if (blocks.size() != totalCount || blocks.empty()) {
		return false;
	}
	if (walls.size() != totalCount || walls.empty()) {
		return false;
	}

	f.write(reinterpret_cast<const char*>(&VERSION), sizeof(VERSION));

	f.write(reinterpret_cast<const char*>(&w), sizeof(w));
	f.write(reinterpret_cast<const char*>(&h), sizeof(h));

	for (size_t i {}; i < totalCount; i++) {
		auto b { toBlockRepresentation(blocks[i]) };
		f.write(reinterpret_cast<const char*>(&b), sizeof(b));

		auto w { toWallRepresentation(walls[i]) };
		f.write(reinterpret_cast<const char*>(&w), sizeof(w));
	}

	return f.good();
}

bool loadMapDataFromFile(std::vector<Block>& blocks, std::vector<Wall>& walls,
    int& w, int& h, const char* fileName) {
	blocks.clear();
	walls.clear();
	w = 0;
	h = 0;

	std::ifstream f { fileName, std::ios::binary };
	if (!f.is_open()) {
		return false;
	}

	int version {};
	f.read(reinterpret_cast<char*>(&version), sizeof(version));

	f.read(reinterpret_cast<char*>(&w), sizeof(w));
	f.read(reinterpret_cast<char*>(&h), sizeof(h));
	if (!f || w <= 0 || h <= 0) {
		return false;
	}

	if (w > 10000 || h > 10000) {
		// probably corrupt data
		return false;
	}

	size_t totalCount { static_cast<size_t>(w) * static_cast<size_t>(h) };
	switch (version) {
	case 1: {
		blocks.resize(totalCount);
		walls.resize(totalCount);

		for (size_t i {}; i < totalCount; i++) {
			BlockSaveRepresentation1 readBlock {};
			WallSaveRepresentation1 readWall {};

			f.read(reinterpret_cast<char*>(&readBlock), sizeof(readBlock));
			f.read(reinterpret_cast<char*>(&readWall), sizeof(readWall));

			blocks[i] = readBlock.toBlock();
			walls[i] = readWall.toWall();
		}

		if (!f) {
			blocks.clear();
			walls.clear();
			w = 0;
			h = 0;
			return false;
		}

		break;
	};

	default:
		// invalid version
		w = 0;
		h = 0;
		return false;
	}

	for (size_t i {}; i < totalCount; i++) {
		blocks[i].sanitize();
		walls[i].sanitize();
	}

	return true;
}

void updateWorldSaving(
    float dt, GameMap& gameMap, EntityHolder& entities, Player& player) {
	entitiesSaveTimer += dt;
	mapSaveTimer += dt;
	worldSaveTimer += dt;

	if (worldSaveTimer >= WORLD_SAVE_DELAY) {
		saveWorld(gameMap, entities, player);
		worldSaveTimer = 0.f;
		entitiesSaveTimer = 0.f;
		mapSaveTimer = 0.f;
	} else if (entitiesSaveTimer >= ENTITIES_SAVE_DELAY) {
		saveEntities(entities, player);
		entitiesSaveTimer = 0.f;
	} else if (gameMap.shouldSave && mapSaveTimer >= MAP_SAVE_DELAY) {
		saveMap(gameMap);
		mapSaveTimer = 0.f;
		gameMap.shouldSave = false;
	}
}

void saveMap(GameMap& gameMap) {
	std::error_code error {};
	std::filesystem::create_directory(SAVES_FOLDER_PATH, error);

	writeMapData(gameMap);
	writeBiomes(gameMap.biomes);

	std::filesystem::rename(MAP_SAVE_PATH_TMP, MAP_SAVE_PATH, error);
	std::filesystem::rename(BIOMES_SAVE_PATH_TMP, BIOMES_SAVE_PATH, error);
}

void saveEntities(EntityHolder& entities, Player& player) {
	std::error_code error {};
	std::filesystem::create_directory(SAVES_FOLDER_PATH, error);

	writePlayer(player);
	writeEntities(entities);

	std::filesystem::rename(PLAYER_SAVE_PATH_TMP, PLAYER_SAVE_PATH, error);
	std::filesystem::rename(ENTITIES_SAVE_PATH_TMP, ENTITIES_SAVE_PATH, error);
}

void saveWorld(GameMap& gameMap, EntityHolder& entities, Player& player) {
	std::error_code error {};
	std::filesystem::create_directory(SAVES_FOLDER_PATH, error);

	writeMapData(gameMap);
	writePlayer(player);
	writeBiomes(gameMap.biomes);
	writeEntities(entities);

	// atomic operation -> will either replace fully or not at all.
	// it's still possible that not all files get renamed in time
	// (e.g. on pc crash)
	std::filesystem::rename(MAP_SAVE_PATH_TMP, MAP_SAVE_PATH, error);
	std::filesystem::rename(PLAYER_SAVE_PATH_TMP, PLAYER_SAVE_PATH, error);
	std::filesystem::rename(BIOMES_SAVE_PATH_TMP, BIOMES_SAVE_PATH, error);
	std::filesystem::rename(ENTITIES_SAVE_PATH_TMP, ENTITIES_SAVE_PATH, error);
}

bool loadWorld(GameMap& gameMap, EntityHolder& entities, Player& player) {
	GameMap loadedMap {};
	EntityHolder loadedEntities {};
	Player loadedPlayer {};

	if (!loadMap(loadedMap)) {
		return false;
	}

	if (!loadBiomes(loadedMap.biomes, loadedMap.w)) {
		return false;
	}

	if (!loadPlayer(loadedPlayer)) {
		return false;
	}

	if (!loadEntities(loadedEntities)) {
		return false;
	}

	std::uint64_t highestId { highestEntityId(loadedEntities) };
	loadedEntities.idHolder.idCounter = std::max(highestId, PLAYER_ID) + 1;

	gameMap = std::move(loadedMap);
	entities = std::move(loadedEntities);
	player = std::move(loadedPlayer);

	return true;
}
