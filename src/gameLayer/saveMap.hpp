#pragma once

#include <vector>

#include "blocks.hpp"
#include "walls.hpp"

bool saveMapDataToFile(const std::vector<Block>& blocks,
    const std::vector<Wall>& walls, int w, int h, const char* fileName);

bool loadMapDataFromFile(std::vector<Block>& blocks, std::vector<Wall>& walls,
    int& w, int& h, const char* fileName);

struct GameMap;
struct Player;
struct EntityHolder;

void updateWorldSaving(
    float dt, GameMap& gameMap, EntityHolder& entities, Player& player);

void saveMap(GameMap& gameMap);

void saveEntities(EntityHolder& entities, Player& player);

void saveWorld(GameMap& gameMap, EntityHolder& entities, Player& player);

bool loadWorld(GameMap& gameMap, EntityHolder& entities, Player& player);
