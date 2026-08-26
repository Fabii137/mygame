#include <cmath>
#include <cstdint>

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <random>
#include <utility>

#include "game.hpp"

#include "assetManager.hpp"
#include "audio.hpp"
#include "background.hpp"
#include "blocks.hpp"
#include "constants.hpp"
#include "entity.hpp"
#include "entityHolder.hpp"
#include "gameMap.hpp"
#include "helpers.hpp"
#include "imgui.h"
#include "items.hpp"
#include "physics.hpp"
#include "random.h"
#include "raylib.h"
#include "raymath.h"
#include "saveMap.hpp"
#include "settings.hpp"
#include "walls.hpp"
#include "worldGenerator.hpp"

#include "entities/droppedItem.hpp"
#include "entities/player.hpp"
#include "entities/slime.hpp"
#include "entities/zombie.hpp"

void Game::spawnZombie(Vector2 position) {
	Zombie zombie {};
	zombie.teleport(position);
	m_Entities.add(std::move(zombie));
}

void Game::spawnSlime(Vector2 position, SlimeType type) {
	Slime slime {};
	slime.slimeType() = type;
	slime.teleport(position);
	m_Entities.add(std::move(slime));
}

void Game::spawnDroppedItem(Vector2 position, std::uint16_t type) {
	position.x += getRandomFloat(m_Rng, -0.1f, 0.1f);
	DroppedItem droppedItem {};
	droppedItem.itemType() = type;
	droppedItem.teleport(position);
	m_Entities.add(std::move(droppedItem));
}

bool Game::init() {
	Audio::init();
	m_AssetManager.loadAll();
	loadSettings();

	WorldGenerator worldGenerator { m_GameMap, m_WorldSettings, m_Seed };
	worldGenerator.generate();

	m_Camera.target = { 20.f, 120.f };
	m_Camera.rotation = 0.f;
	m_Camera.zoom = 100.f;

	m_Player.teleport({ 20.f, 120.f });

	return true;
}

bool Game::update() {
	float dt { GetFrameTime() };
	if (dt > 1.f / 5.f) {
		dt = 1.f / 5.f;
	}

	if (IsKeyPressed(KEY_F10)) {
		m_ShowImGui = !m_ShowImGui;
	}

	if (m_Failed) {
		return false;
	}

	updateAudio(dt);
	updateSettings();
	updateEnemySpawning(dt);
	updatePlayer(dt);
	updateCamera();
	updateEntities(dt);
	updateStructureSelection();
	updateWorldEditing();
	updateBackground(dt);

	ClearBackground({ 75, 75, 150, 255 });
	render();

	DrawFPS(10, 10);
	return true;
}

void Game::close() { std::cout << "\n\nCLOSED!!!!!!!!!\n\n"; }

void Game::updateCamera() {
	int scrWidth { GetScreenWidth() };
	int scrHeight { GetScreenHeight() };

	m_Camera.target = m_Player.position();
	m_Camera.offset = { scrWidth * 0.5f, scrHeight * 0.5f };

	// clamp camera
	float viewWidth { scrWidth / m_Camera.zoom };
	float viewHeight { scrHeight / m_Camera.zoom };

	if (viewWidth <= m_GameMap.w) {
		float minX { viewWidth * 0.5f };
		float maxX { m_GameMap.w - viewWidth * 0.5f };
		m_Camera.target.x = std::clamp(m_Camera.target.x, minX, maxX);
	} else {
		m_Camera.target.x = m_GameMap.w * 0.5f;
	}

	if (viewHeight <= m_GameMap.h) {
		float minY { viewHeight * 0.5f };
		float maxY(m_GameMap.h - viewHeight * 0.5f);
		m_Camera.target.y = std::clamp(m_Camera.target.y, minY, maxY);
	} else {
		m_Camera.target.y = m_GameMap.h * 0.5f;
	}
}

void Game::updateAudio(float dt) {
	Audio::update(dt);

	using MusicType = Audio::Musics;

	MusicType type {};
	if (m_Player.position().y > 180.f) {
		type = MusicType::MusicCave;
	} else {
		for (const Biome& biome : m_GameMap.biomes) {
			if (m_Player.position().x >= biome.startX
			    && m_Player.position().x < biome.endX) {
				switch (biome.type) {
				case Biome::Forest:
					type = MusicType::MusicForest;
					break;
				case Biome::Desert:
					type = MusicType::MusicDesert;
					break;
				case Biome::Snow:
					type = MusicType::MusicSnow;
					break;
				default:
					break;
				}
				break;
			}
		}
	}

	Audio::playMusic(type);
}

void Game::updatePlayer(float dt) {
	EntityUpdateData updateData {
		m_Player.position(),
		m_Rng,
		m_Entities,
		0,
		m_CreativeMode,
	};

	m_Player.update(dt, updateData);
	m_Player.updatePhysics(dt, m_GameMap, !m_CreativeMode);
}

void Game::updateEntities(float dt) {
	Vector2 mousePosWorld { getMousePosWorld() };

	for (auto it { m_Entities.entities.begin() };
	    it != m_Entities.entities.end();) {
		EntityUpdateData updateData {
			m_Player.position(),
			m_Rng,
			m_Entities,
			it->first,
			m_CreativeMode,
		};

		auto& entity { it->second };
		// TODO: move somewhere else
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			if (entity->physics().transform.intersectPoint(mousePosWorld)) {
				entity->takeDamage(1.f);
			}
		}

		float distanceToPlayer { Vector2Distance(
			  entity->position(), m_Player.position()) };
		bool inBounds { m_GameMap.inBounds(entity->position()) };

		bool shouldDespawn { false };
		if (it->second->isEnemy()) {
			if (distanceToPlayer >= ENEMY_DESPAWN_DISTANCE) {
				it->second->timeOutsideDespawnRange() += dt;
			} else {
				it->second->timeOutsideDespawnRange() = 0.f;
			}
			shouldDespawn
			    = it->second->timeOutsideDespawnRange() >= ENEMY_DESPAWN_DELAY;
		}

		bool shouldKill { !it->second->update(dt, updateData)
			|| it->second->health() <= 0.f || !inBounds || shouldDespawn };
		if (shouldKill) {
			if (entity->type() == EntityType::Slime) {
				spawnDroppedItem(it->second->position(), Block::GoldBlock);
			} else if (entity->type() == EntityType::Zombie) {
				spawnDroppedItem(it->second->position(), Block::IronBlock);
			}
			it = m_Entities.entities.erase(it);
			continue;
		}

		it->second->updatePhysics(dt, m_GameMap);
		++it;
	}
}

std::optional<Vector2> Game::findGroundSpawnPosition(float x) {
	int blockX { static_cast<int>(std::floor(x)) };

	if (blockX < 0 || blockX >= m_GameMap.w) {
		return std::nullopt;
	}

	int playerY { static_cast<int>(std::floor(m_Player.position().y)) };
	int startY { std::max(
		  1, playerY - static_cast<int>(ENEMY_SPAWN_MAX_DISTANCE)) };
	int endY { std::min(
		  m_GameMap.h - 2, playerY + static_cast<int>(ENEMY_SPAWN_MAX_DISTANCE)) };

	for (int y { startY }; y <= endY; ++y) {
		Block* ground { m_GameMap.blockSafe(blockX, y) };
		Block* above { m_GameMap.blockSafe(blockX, y - 1) };
		Block* above2 { m_GameMap.blockSafe(blockX, y - 2) };

		if (!ground || !above || !above2) {
			continue;
		}

		if (ground->type != Block::Air && above->type == Block::Air
		    && above2->type == Block::Air) {
			return Vector2 {
				static_cast<float>(blockX) + 0.5f,
				static_cast<float>(y) - 0.5f,
			};
		}
	}

	return std::nullopt;
}

Vector2 Game::getMousePosWorld() const {
	return GetScreenToWorld2D(GetMousePosition(), m_Camera);
}

bool Game::canPlaceBlock(const MapCell& hoveredCell) {
	if (!hoveredCell.block) {
		return false;
	}

	Vector2 blockCenter {
		static_cast<float>(hoveredCell.x) + 0.5f,
		static_cast<float>(hoveredCell.y) + 0.5f,
	};

	Transform2D blockTransform {
		.pos = blockCenter,
		.w = 1,
		.h = 1,
	};

	return !m_Player.physics().transform.intersectTransform(
	    blockTransform, -0.00005f);
}

void Game::updateEnemySpawning(float dt) {
	m_EnemySpawnTimer -= dt;

	if (m_EnemySpawnTimer > 0.f) {
		return;
	}

	m_EnemySpawnTimer = ENEMY_SPAWN_INTERVAL;

	std::size_t enemyCount {};
	for (const auto& [id, entity] : m_Entities.entities) {
		if (entity->isEnemy()) {
			enemyCount++;
		}
	}

	if (enemyCount >= MAX_ENEMIES) {
		return;
	}

	float distance {
		getRandomFloat(m_Rng, ENEMY_SPAWN_MIN_DISTANCE, ENEMY_SPAWN_MAX_DISTANCE),
	};
	float direction { getRandomChance(m_Rng, 0.5f) ? -1.f : 1.f };
	float spawnX { m_Player.position().x + distance * direction };

	auto spawnPosition { findGroundSpawnPosition(spawnX) };
	if (!spawnPosition.has_value()) {
		return;
	}

	spawnSlime(spawnPosition.value(), SlimeType::Green);
}

void Game::updateWorldEditing() {
	if (m_ShowImGui) {
		return;
	}

	MapCell hoveredCell { m_GameMap.hoveredCell(getMousePosWorld()) };
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		if (m_EditMode == EditMode::Blocks) {
			if (hoveredCell.block) {
				if (hoveredCell.block->type) {
					Vector2 blockCenter {
						static_cast<float>(hoveredCell.x) + 0.5f,
						static_cast<float>(hoveredCell.y) + 0.5f,
					};
					spawnDroppedItem(blockCenter, hoveredCell.block->type);
					Audio::playSound(Audio::BreakBlock);
				}
				*hoveredCell.block = {};
			}
		} else {
			if (hoveredCell.wall) {
				*hoveredCell.wall = {};
			}
		}
	}

	if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
		if (m_EditMode == EditMode::Blocks) {
			if (hoveredCell.block) {
				m_CreativeSelectedBlock = hoveredCell.block->type;
			}
		} else {
			if (hoveredCell.wall) {
				m_CreativeSelectedWall = hoveredCell.wall->type;
			}
		}
	}

	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
		if (m_EditMode == EditMode::Blocks) {
			if (canPlaceBlock(hoveredCell)) {
				hoveredCell.block->type = m_CreativeSelectedBlock;
				Audio::playSound(Audio::PlaceBlock);
			}
		} else {
			if (hoveredCell.wall) {
				hoveredCell.wall->type = m_CreativeSelectedWall;
				Audio::playSound(Audio::PlaceBlock);
			}
		}
	}
}

void Game::updateStructureSelection() {
	if (!m_ShowImGui) {
		return;
	}

	MapCell hoveredCell { m_GameMap.hoveredCell(getMousePosWorld()) };
	if (IsKeyPressed(KEY_ONE)) {
		m_SelectionStart = {
			static_cast<float>(hoveredCell.x),
			static_cast<float>(hoveredCell.y),
		};
	}
	if (IsKeyPressed(KEY_TWO)) {
		m_SelectionEnd = {
			static_cast<float>(hoveredCell.x),
			static_cast<float>(hoveredCell.y),
		};
	}
	if (IsKeyPressed(KEY_THREE)) {
		m_CopyStructure.pasteIntoMap(m_GameMap,
		    { static_cast<float>(hoveredCell.x),
		        static_cast<float>(hoveredCell.y) });
	}

	if (m_SelectionStart.x > m_SelectionEnd.x) {
		std::swap(m_SelectionStart.x, m_SelectionEnd.x);
	}
	if (m_SelectionStart.y > m_SelectionEnd.y) {
		std::swap(m_SelectionStart.y, m_SelectionEnd.y);
	}
}

void Game::updateBackground(float dt) {
	BackgroundType type {};

	if (m_Player.position().y > 180.f) {
		type = BackgroundType::Cave;
	} else {
		for (const Biome& biome : m_GameMap.biomes) {
			if (m_Player.position().x >= biome.startX
			    && m_Player.position().x < biome.endX) {
				switch (biome.type) {
				case Biome::Forest:
					type = BackgroundType::Forest;
					break;
				case Biome::Desert:
					type = BackgroundType::Desert;
					break;
				case Biome::Snow:
					type = BackgroundType::Snow;
					break;
				default:
					break;
				}
				break;
			}
		}
	}

	m_Background.setBackground(type);

	m_Background.update(dt);
}

void Game::render() {
	renderBackground();
	BeginMode2D(m_Camera);

	Vector2 screenSize {
		static_cast<float>(GetScreenWidth()),
		static_cast<float>(GetScreenHeight()),
	};
	Vector2 topLeftView { GetScreenToWorld2D({ 0, 0 }, m_Camera) };
	Vector2 bottomRightView { GetScreenToWorld2D(screenSize, m_Camera) };

	int startXView { static_cast<int>(std::floor(topLeftView.x - 1)) };
	int endXView { static_cast<int>(std::ceil(bottomRightView.x + 1)) };
	int startYView { static_cast<int>(std::floor(topLeftView.y - 1)) };
	int endYView { static_cast<int>(std::ceil(bottomRightView.y + 1)) };

	startXView = std::clamp(startXView, 0, m_GameMap.w - 1);
	endXView = std::clamp(endXView, 0, m_GameMap.w - 1);

	startYView = std::clamp(startYView, 0, m_GameMap.h - 1);
	endYView = std::clamp(endYView, 0, m_GameMap.h - 1);

	for (int y { startYView }; y <= endYView; y++) {
		for (int x { startXView }; x <= endXView; x++) {
			Wall& wall { m_GameMap.wallUnsafe(x, y) };
			Block& block { m_GameMap.blockUnsafe(x, y) };
			Rectangle dest {
				static_cast<float>(x),
				static_cast<float>(y),
				1.f,
				1.f,
			};

			int variant { getTextureVariant(x, y) };

			if (wall.type != static_cast<Wall::Type>(Block::Air)) {
				int atlasX { wall.type - static_cast<Wall::Type>(Block::BLOCKS_END)
					+ static_cast<Wall::Type>(Block::BLOCKS_COUNT) };
				drawTextureAtlas(m_AssetManager.textures, atlasX, variant, dest);
			}
			if (block.type == Block::WoodLog) {
				int col { getTreeAtlasColumn(x, y) };
				drawTextureAtlas(m_AssetManager.treeTextures, col, variant, dest);
			} else if (block.type != Block::Air) {
				drawTextureAtlas(m_AssetManager.textures, block.type, variant, dest);
			}
		}
	}

	MapCell hoveredCell { m_GameMap.hoveredCell(getMousePosWorld()) };
	// drawTexture(m_AssetManager.frame,
	//             {0.f, 0.f, static_cast<float>(m_AssetManager.frame.width),
	//              static_cast<float>(m_AssetManager.frame.height)},
	//             {static_cast<float>(hoveredCell.x),
	//              static_cast<float>(hoveredCell.y), 1.f, 1.f});

	if (m_EditMode == EditMode::Blocks && hoveredCell.block) {
		Rectangle dest { static_cast<float>(hoveredCell.x),
			static_cast<float>(hoveredCell.y), 1.f, 1.f };
		drawTextureAtlas(m_AssetManager.textures, m_CreativeSelectedBlock, 0, dest,
		    Fade(WHITE, 0.5f));
	} else if (m_EditMode == EditMode::Walls && hoveredCell.wall) {
		Rectangle dest { static_cast<float>(hoveredCell.x),
			static_cast<float>(hoveredCell.y), 1.f, 1.f };
		drawTextureAtlas(m_AssetManager.textures, m_CreativeSelectedWall, 0, dest,
		    Fade(WHITE, 0.5f));
	}

	if (m_ShowImGui) {
		Rectangle rect { m_SelectionStart.x, m_SelectionStart.y,
			m_SelectionEnd.x - m_SelectionStart.x,
			m_SelectionEnd.y - m_SelectionStart.y };
		rect.width++;
		rect.height++;

		DrawRectangleLinesEx(rect, 0.1f, { 20, 101, 250, 145 });
	}

	for (const auto& [id, entity] : m_Entities.entities) {
		entity->render(m_AssetManager);
	}

	m_Player.render(m_AssetManager);

	if (m_ShowImGui) {
		renderImGuiWindows();
	}

	EndMode2D();

	Color ambientTint { m_Background.getAmbientTint() };
	DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ambientTint);
}

void Game::renderBackground() {
	Vector2 mapSize { static_cast<float>(m_GameMap.w),
		static_cast<float>(m_GameMap.h) };
	m_Background.draw(m_AssetManager, m_Camera, mapSize);
}

int Game::getTextureVariant(int x, int y) {
	constexpr int totalVariants { 4 };

	std::uint32_t hash { (static_cast<std::uint32_t>(x) * 73856093)
		^ (static_cast<std::uint32_t>(y) * 19349663) };
	return hash % totalVariants;
}

int Game::getTreeAtlasColumn(int x, int y) {
	const Block* left { m_GameMap.blockSafe(x - 1, y) };
	const Block* right { m_GameMap.blockSafe(x + 1, y) };
	const Block* top { m_GameMap.blockSafe(x, y - 1) };
	const Block* bottom { m_GameMap.blockSafe(x, y + 1) };

	bool leavesLeft { left && left->type == Block::Leaves };
	bool leavesRight { right && right->type == Block::Leaves };
	bool leavesTop { top && top->type == Block::Leaves };
	bool logTop { top && top->type == Block::WoodLog };
	bool groundBottom { bottom && bottom->type != Block::Air
		&& bottom->type != Block::WoodLog && bottom->type != Block::Leaves };

	if (leavesTop) {
		return 5;
	}
	if (leavesLeft && leavesRight) {
		return 1;
	}
	if (leavesRight) {
		return 2;
	}
	if (leavesLeft) {
		return 3;
	}
	if (!logTop && groundBottom) {
		return 7;
	}
	if (!logTop) {
		return 6;
	}
	if (groundBottom) {
		return 4;
	}

	return 0;
}

void Game::renderImGuiWindows() {
	ImGui::Begin("Game Control");
	ImGui::SliderFloat("Camera Zoom", &m_Camera.zoom, 1, 150);
	ImGui::SliderFloat("Player Speed", &m_Player.speed(), 5, 100);
	ImGui::Checkbox("Creative Mode", &m_CreativeMode);
	if (ImGui::Button("Spawn Random Slime")) {
		int type { getRandomInt(m_Rng, 0, 2) };
		spawnSlime({ 18, 60 }, static_cast<SlimeType>(type));
	}
	if (ImGui::Button("Spawn Zombie")) {
		spawnZombie({ 18, 60 });
	}
	ImGui::Separator();

	if (ImGui::Button("Save World")) {
		saveWorld(m_GameMap, m_Entities, m_Player);
	}

	if (ImGui::Button("Load World")) {
		if (!loadWorld(m_GameMap, m_Entities, m_Player)) {
			m_Failed = true;
		}
	}

	if (ImGui::Button("Load Texture Pack")) {
		m_AssetManager.loadAll(RESOURCES_PATH "../texturePacks/hdtextures");
	}
	if (ImGui::Button("Unload Texture Pack")) {
		m_AssetManager.loadAll();
	}
	ImGui::Separator();

	if (ImGui::Button("Copy")) {
		m_CopyStructure.copyFromMap(m_GameMap, m_SelectionStart, m_SelectionEnd);
	}
	ImGui::InputText("File name", m_SaveName, sizeof(m_SaveName));
	if (ImGui::Button("Save to file")) {
		std::string path { RESOURCES_PATH "structures/" };
		path += m_SaveName;
		path += ".bin";

		saveMapDataToFile(m_CopyStructure.mapData, m_CopyStructure.wallData,
		    m_CopyStructure.w, m_CopyStructure.h, path.c_str());
	}
	if (ImGui::Button("Load from file")) {
		std::string path { RESOURCES_PATH "structures/" };
		path += m_SaveName;
		path += ".bin";

		loadMapDataFromFile(m_CopyStructure.mapData, m_CopyStructure.wallData,
		    m_CopyStructure.w, m_CopyStructure.h, path.c_str());
	}
	ImGui::Separator();

	ImGui::Text("Settings");
	ImGui::SliderFloat("Master Volume", &getSettings().masterVolume, 0.f, 1.f);
	ImGui::SliderFloat("Sounds Volume", &getSettings().soundsVolume, 0.f, 1.f);
	ImGui::SliderFloat("Music Volume", &getSettings().musicVolume, 0.f, 1.f);
	ImGui::Separator();

	ImGui::Text("General");
	ImGui::Text("Seed: %u", m_Seed);
	if (ImGui::Button("Random Seed")) {
		static std::ranlux24_base rng { std::random_device {}() };
		m_Seed = getRandomInt(rng, 0, std::numeric_limits<int>::max());
	}
	ImGui::Separator();

	if (ImGui::CollapsingHeader("Creative")) {
		if (ImGui::RadioButton("Block Mode", m_EditMode == EditMode::Blocks)) {
			m_EditMode = EditMode::Blocks;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Wall Mode", m_EditMode == EditMode::Walls)) {
			m_EditMode = EditMode::Walls;
		}

		auto addTexButton = [&](int i, std::function<void(void)> onClick) {
			auto atlas { getTextureAtlas(
				  i, 0, Constants::CELL_SIZE, Constants::CELL_SIZE) };
			atlas.x /= m_AssetManager.textures.width;
			atlas.width /= m_AssetManager.textures.width;
			atlas.y /= m_AssetManager.textures.height;
			atlas.height /= m_AssetManager.textures.height;

			ImGui::PushID(i);
			ImTextureID tex { reinterpret_cast<ImTextureID>(
				  static_cast<intptr_t>(m_AssetManager.textures.id)) };
			if (ImGui::ImageButton(tex, { 35, 35 }, { atlas.x, atlas.y },
			        { atlas.x + atlas.width, atlas.y + atlas.height })) {
				onClick();
			}
			ImGui::PopID();

			if (i % 10 != 0) {
				ImGui::SameLine();
			}
		};

		if (ImGui::CollapsingHeader("Blocks")) {
			for (std::uint16_t i {}; i < Block::BLOCKS_COUNT; i++) {
				addTexButton(i,
				    [&]() { m_CreativeSelectedBlock = static_cast<Block::Type>(i); });
			}
		}
		ImGui::Separator();
		if (ImGui::CollapsingHeader("Walls")) {
			for (std::uint16_t i { Block::BLOCKS_COUNT }; i < Wall::WALLS_COUNT;
			    i++) {
				addTexButton(
				    i, [&]() { m_CreativeSelectedWall = static_cast<Wall::Type>(i); });
			}
		}
	}

	ImGui::Separator();

	if (ImGui::CollapsingHeader("World Generation")) {
		ImGui::Text("Mountain Noise");
		ImGui::SliderFloat("Mountain Frequency",
		    &m_WorldSettings.mountain.frequency, 0.001f, 0.1f);
		ImGui::SliderInt(
		    "Mountain Octaves", &m_WorldSettings.mountain.octaves, 1, 8);
		ImGui::SliderInt(
		    "Mountain Height Min", &m_WorldSettings.mountainHeightStart, 0, 250);
		ImGui::SliderInt(
		    "Mountain Height Max", &m_WorldSettings.mountainHeightEnd, 0, 300);
		ImGui::Separator();

		ImGui::Text("Plains Noise");
		ImGui::SliderFloat(
		    "Plains Frequency", &m_WorldSettings.plains.frequency, 0.001f, 0.1f);
		ImGui::SliderInt("Plains Octaves", &m_WorldSettings.plains.octaves, 1, 8);
		ImGui::SliderInt(
		    "Plains Height Min", &m_WorldSettings.plainsHeightStart, 0, 250);
		ImGui::SliderInt(
		    "Plains Height Max", &m_WorldSettings.plainsHeightEnd, 0, 300);
		ImGui::Separator();

		ImGui::Text("Blend Noise");
		ImGui::SliderFloat(
		    "Blend Frequency", &m_WorldSettings.blend.frequency, 0.0001f, 0.05f);
		ImGui::SliderInt("Blend Octaves", &m_WorldSettings.blend.octaves, 1, 8);
		ImGui::Separator();

		ImGui::Text("Stone Noise");
		ImGui::SliderFloat(
		    "Stone Frequency", &m_WorldSettings.stone.frequency, 0.001f, 0.1f);
		ImGui::SliderInt("Stone Octaves", &m_WorldSettings.stone.octaves, 1, 8);
		ImGui::SliderInt(
		    "Stone Offset Min", &m_WorldSettings.stoneOffsetStart, -100, 100);
		ImGui::SliderInt(
		    "Stone Offset Max", &m_WorldSettings.stoneOffsetEnd, -100, 100);
		ImGui::Separator();

		ImGui::Text("Cave Noise");
		ImGui::SliderFloat(
		    "Cave1 Frequency", &m_WorldSettings.cave1.frequency, 0.001f, 0.1f);
		ImGui::SliderInt("Cave1 Octaves", &m_WorldSettings.cave1.octaves, 1, 8);
		ImGui::SliderFloat(
		    "Cave2 Frequency", &m_WorldSettings.cave2.frequency, 0.001f, 0.1f);
		ImGui::SliderInt("Cave2 Octaves", &m_WorldSettings.cave2.octaves, 1, 8);
		ImGui::SliderFloat(
		    "Min Cave Noise", &m_WorldSettings.minCaveNoise, 0.f, 1.f);
	}
	ImGui::Separator();

	if (ImGui::Button("Regenerate World")) {
		WorldGenerator worldGenerator { m_GameMap, m_WorldSettings, m_Seed };
		worldGenerator.generate();
	}

	ImGui::End();
}
