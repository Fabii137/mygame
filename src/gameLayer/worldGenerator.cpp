#include <cmath>
#include <cstdint>
#include <cstdlib>

#include <algorithm>
#include <optional>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "worldGenerator.hpp"

#include "asserts.h"
#include "blocks.hpp"
#include "gameMap.hpp"
#include "random.h"
#include "raylib.h"
#include "saveMap.hpp"
#include "structure.hpp"

#include "FastNoise/FastNoise.h"

NoiseData::NoiseData(std::vector<float> mountain, std::vector<float> plains,
    std::vector<float> blend, std::vector<float> stone,
    std::vector<float> cave1, std::vector<float> cave2)
    : mountain(std::move(mountain))
    , plains(std::move(plains))
    , blend(std::move(blend))
    , stone(std::move(stone))
    , cave1(std::move(cave1))
    , cave2(std::move(cave2)) { }

WorldGenerator::WorldGenerator(
    GameMap& gameMap, const WorldSettings& settings, std::uint32_t seed)
    : m_GameMap(gameMap)
    , m_Settings(settings)
    , m_Seed(seed) { }

void WorldGenerator::generate() {
	m_GameMap.create(WIDTH, HEIGHT);

	std::ranlux24_base rng { m_Seed++ };

	NoiseData noiseData { generateNoiseData() };

	TerrainData terrainData { generateBaseTerrain(noiseData) };
	generateBiomeMap(terrainData, rng);
	applyBiomes(terrainData);
	generateCaves(noiseData);
	generateOres(terrainData, rng);
	generateWorms(rng);
	generateStructures(terrainData, rng);

	m_GameMap.biomes = std::move(terrainData.biomes);
}

TerrainData WorldGenerator::generateBaseTerrain(const NoiseData& noiseData) {
	std::vector<int> surfaceHeights(WIDTH);
	std::vector<int> stoneHeights(WIDTH);

	for (int x {}; x < WIDTH; x++) {
		float mountainHeight {
			std::lerp(static_cast<float>(m_Settings.mountainHeightStart),
			    static_cast<float>(m_Settings.mountainHeightEnd),
			    noiseData.mountain[x]),
		};
		float plainsHeight {
			std::lerp(static_cast<float>(m_Settings.plainsHeightStart),
			    static_cast<float>(m_Settings.plainsHeightEnd), noiseData.plains[x]),
		};

		int surfaceHeight { static_cast<int>(
			  std::lerp(mountainHeight, plainsHeight, noiseData.blend[x])) };

		int stoneOffset { static_cast<int>(std::lerp(m_Settings.stoneOffsetStart,
			  m_Settings.stoneOffsetEnd, noiseData.stone[x])) };
		int stoneHeight { surfaceHeight + stoneOffset };

		surfaceHeights[x] = surfaceHeight;
		stoneHeights[x] = stoneHeight;

		for (int y {}; y < HEIGHT; y++) {
			Block& block { m_GameMap.blockUnsafe(x, y) };
			if (y < surfaceHeight) {
				block.type = Block::Air;
			} else {
				block.type = Block::Dirt;
			}
		}
	}

	return { surfaceHeights, stoneHeights };
}

void WorldGenerator::generateBiomeMap(
    TerrainData& terrainData, std::ranlux24_base& rng) {
	std::optional<Biome::Type> previous {};

	auto getRandomBiome = [&]() {
		Biome::Type type {};
		do {
			type = static_cast<Biome::Type>(
			    getRandomInt(rng, 0, Biome::BIOMES_COUNT - 1));
		} while (previous.has_value() && previous.value() == type);

		return type;
	};

	int x {};
	while (x < WIDTH) {
		int length { getRandomInt(rng, 100, 250) };
		int end { std::min(x + length, WIDTH) };

		Biome biome {
			.type = getRandomBiome(),
			.startX = x,
			.endX = end,
		};
		terrainData.biomes.push_back(biome);

		previous = biome.type;
		x = end;
	}
}

const Biome* WorldGenerator::getBiomeAt(
    const TerrainData& terrainData, int x) const {
	for (const Biome& biome : terrainData.biomes) {
		if (x >= biome.startX && x < biome.endX) {
			return &biome;
		}
	}

	return nullptr;
}

bool WorldGenerator::isBiomeAllowed(
    const std::vector<Biome::Type>& allowed, const Biome* biome) {
	if (!biome) {
		return false;
	}

	if (allowed.empty()) {
		return true;
	}

	return std::ranges::find(allowed, biome->type) != allowed.end();
}

bool WorldGenerator::isValidSurfaceBlock(const Biome& biome, int x, int y) {
	const Block* block { m_GameMap.blockSafe(x, y) };
	if (!block) {
		return false;
	}

	switch (biome.type) {
	case Biome::Forest:
		return block->type == Block::GrassBlock;
	case Biome::Desert:
		return block->type == Block::Sand;
	case Biome::Snow:
		return block->type == Block::Snow;
	default:
		return false;
	}
}

void WorldGenerator::applyBiomes(const TerrainData& terrainData) {
	for (const Biome& biome : terrainData.biomes) {
		for (int x { biome.startX }; x < biome.endX; x++) {
			switch (biome.type) {
			case Biome::Forest:
				generateForestColumn(x, biome, terrainData);
				break;
			case Biome::Desert:
				generateDesertColumn(x, biome, terrainData);
				break;
			case Biome::Snow:
				generateSnowColumn(x, biome, terrainData);
				break;
			default:
				permaAssertCommentDevelopment(false, "Unknown biome");
				break;
			}
		}
	}
}

void WorldGenerator::generateDesertColumn(
    int x, const Biome& biome, const TerrainData& terrainData) {
	int desertMid { (biome.startX + biome.endX) / 2 };
	int desertHalfWidth { (biome.endX - biome.startX) / 2 };

	int surfaceHeight { terrainData.surfaceHeights[x] };
	int stoneHeight { terrainData.stoneHeights[x] };

	int distanceFromDesertMid { std::abs(x - desertMid) };

	float desertDistance { 1
		- distanceFromDesertMid / static_cast<float>(desertHalfWidth) };
	desertDistance = std::clamp(desertDistance, 0.f, 1.f);
	desertDistance = std::pow(desertDistance, 0.5f);

	int stoneStart { 10 + stoneHeight };
	int stoneDepth { 20 + stoneHeight };
	int stoneY { static_cast<int>(stoneStart + desertDistance * stoneDepth) };

	for (int y { surfaceHeight }; y < HEIGHT; y++) {
		Block& block { m_GameMap.blockUnsafe(x, y) };
		Wall& wall { m_GameMap.wallUnsafe(x, y) };

		if (y >= stoneY) {
			block.type = Block::Stone;
			wall.type = Wall::DirtWall;
		} else if (y >= stoneHeight) {
			block.type = Block::SandStone;
			wall.type = Wall::SandStoneWall;
		} else {
			block.type = Block::Sand;
		}
	}
}

void WorldGenerator::generateForestColumn(
    int x, const Biome& biome, const TerrainData& terrainData) {
	int surfaceHeight { terrainData.surfaceHeights[x] };
	int stoneHeight { terrainData.stoneHeights[x] };

	for (int y { surfaceHeight }; y < HEIGHT; y++) {
		Block& block { m_GameMap.blockUnsafe(x, y) };
		Wall& wall { m_GameMap.wallUnsafe(x, y) };

		if (y > stoneHeight) {
			block.type = Block::Stone;
			wall.type = Wall::DirtWall;
		} else if (y == surfaceHeight) {
			block.type = Block::GrassBlock;
		} else {
			block.type = Block::Dirt;
		}
	}
}

void WorldGenerator::generateSnowColumn(
    int x, const Biome& biome, const TerrainData& terrainData) {
	int snowMid { (biome.startX + biome.endX) / 2 };
	int snowHalfWidth { (biome.endX - biome.startX) / 2 };

	int surfaceHeight { terrainData.surfaceHeights[x] };
	int stoneHeight { terrainData.stoneHeights[x] };

	int distanceFromSnowMid { std::abs(x - snowMid) };

	float snowDistance { 1
		- distanceFromSnowMid / static_cast<float>(snowHalfWidth) };
	snowDistance = std::clamp(snowDistance, 0.f, 1.f);
	snowDistance = std::pow(snowDistance, 0.5f);

	int stoneStart { 10 + stoneHeight };
	int stoneDepth { 20 + stoneHeight };
	int stoneY { static_cast<int>(stoneStart + snowDistance * stoneDepth) };

	for (int y { surfaceHeight }; y < HEIGHT; y++) {
		Block& block { m_GameMap.blockUnsafe(x, y) };
		Wall& wall { m_GameMap.wallUnsafe(x, y) };

		if (y >= stoneY) {
			block.type = Block::Stone;
			wall.type = Wall::DirtWall;
		} else if (y >= stoneHeight) {
			block.type = Block::Snow;
			wall.type = Wall::SnowWall;
		} else {
			block.type = Block::Snow;
		}
	}
}

void WorldGenerator::generateCaves(const NoiseData& noiseData) {
	auto screenBlend
	    = [](float a, float b) { return 1.f - (1.f - a) * (1.f - b); };

	auto getCaveNoise = [](const std::vector<float>& noise, int x, int y) {
		return noise[y * WIDTH + x];
	};

	for (int x {}; x < WIDTH; x++) {
		for (int y {}; y < HEIGHT; y++) {
			if (y <= m_Settings.plainsHeightStart + 20) {
				continue;
			}

			float cave1 { getCaveNoise(noiseData.cave1, x, y) };
			float cave2 { getCaveNoise(noiseData.cave2, x, y) };
			float caveValue { screenBlend(cave1, cave2) };

			if (caveValue > m_Settings.minCaveNoise) {
				m_GameMap.blockUnsafe(x, y).type = Block::Air;
			}
		}
	}
}

void WorldGenerator::generateWorms(std::ranlux24_base& rng) {
	auto spawnWorm
	    = [&](Vector2 start, Vector2 direction, int length, float radius) {
		      float x { start.x };
		      float y { start.y };
		      int changeDirectionTime { getRandomInt(rng, 5, 20) };

		      for (int j {}; j < length; j++) {
			      int intRadius { static_cast<int>(std::ceil(radius)) };
			      for (int dx { -intRadius }; dx < intRadius; dx++) {
				      for (int dy { -intRadius }; dy < intRadius; dy++) {
					      float distSqr { static_cast<float>(dx * dx + dy * dy) };
					      if (distSqr <= radius * radius) {
						      int targetX { static_cast<int>(x + dx) };
						      int targetY { static_cast<int>(y + dy) };
						      Block* block { m_GameMap.blockSafe(targetX, targetY) };
						      if (block) {
							      block->type = Block::Air;
						      }
					      }
				      }
			      }

			      changeDirectionTime--;
			      if (changeDirectionTime <= 0) {
				      changeDirectionTime = getRandomInt(rng, 5, 20);
				      float keepFactor { getRandomChance(rng, 0.7f) ? 0.8f : 0.2f };

				      direction.x = direction.x * keepFactor
				          + getRandomFloat(rng, -1, 1) * (1.f - keepFactor);
				      direction.y = direction.y * keepFactor
				          + getRandomFloat(rng, -1, 1) * (1.f - keepFactor);
			      }

			      x += direction.x * 1.5f;
			      y += direction.y * 1.5f;

			      radius += getRandomFloat(rng, -0.2, 0.2f);
			      radius = std::clamp(radius, 2.2f, 8.5f);
		      }
	      };

	for (int i {}; i < 15; i++) {
		float x { static_cast<float>(getRandomInt(rng, 10, WIDTH - 10)) };
		float y { static_cast<float>(
			  getRandomInt(rng, m_Settings.plainsHeightEnd - 20, HEIGHT - 10)) };

		float dirX { getRandomFloat(rng, -1, 1) };
		float dirY { getRandomFloat(rng, -1, 1) };

		int wormLength { getRandomInt(rng, 150, 400) };
		float radius { 2.5f };
		spawnWorm({ x, y }, { dirX, dirY }, wormLength, radius);
	}
}

void WorldGenerator::generateStructures(
    const TerrainData& terrainData, std::ranlux24_base& rng) {
	for (const Biome& biome : terrainData.biomes) {
		for (int x { biome.startX }; x < biome.endX; x++) {
			const StructureSettings* selected {};

			for (const StructureSettings& settings : m_Settings.structures) {
				if (settings.variants.empty()) {
					continue;
				}

				if (!isBiomeAllowed(settings.biomes, &biome)) {
					continue;
				}

				if (getRandomChance(rng, settings.spawnChance)) {
					selected = &settings;
					break;
				}
			}

			if (!selected) {
				continue;
			}

			const std::string& variant {
				selected->variants[getRandomInt(rng, 0, selected->variants.size() - 1)],
			};

			// TODO: avoid loading the same file multiple times (preload all files?)
			Structure structure {};
			std::string path {
				std::string(RESOURCES_PATH) + "structures/" + variant + ".bin",
			};
			loadMapDataFromFile(structure.mapData, structure.wallData, structure.w,
			    structure.h, path.c_str());

			int surfaceHeight { terrainData.surfaceHeights[x] };
			if (!isValidSurfaceBlock(biome, x, surfaceHeight)) {
				continue;
			}

			int halfWidth { static_cast<int>(std::floor(structure.w / 2.f)) };
			int start { x - halfWidth };
			int end { start + structure.w - 1 };

			if (start < biome.startX || end >= biome.endX) {
				// dont allow the structure to cross biome boundary
				continue;
			}

			Vector2 spawnPos {
				static_cast<float>(start),
				static_cast<float>(surfaceHeight - structure.h),
			};
			structure.pasteIntoMap(m_GameMap, spawnPos);

			x = end;
		}
	}
}

void WorldGenerator::generateOres(
    const TerrainData& terrainData, std::ranlux24_base& rng) {
	auto isValidOreBlock = [&](const OreSettings& settings, int x, int y) {
		if (!isBiomeAllowed(settings.biomes, getBiomeAt(terrainData, x))) {
			return false;
		}

		Block* block { m_GameMap.blockSafe(x, y) };
		if (!block) {
			return false;
		}

		bool canReplace { settings.canReplace.empty()
			|| std::ranges::find(settings.canReplace, block->type)
			    != settings.canReplace.end() };
		return canReplace;
	};

	auto generateVein = [&](const OreSettings& settings, Vector2 spawnPos) {
		int size { getRandomInt(rng, settings.minVeinSize, settings.maxVeinSize) };
		for (int j {}; j < size; j++) {
			int targetX { static_cast<int>(spawnPos.x) };
			int targetY { static_cast<int>(spawnPos.y) };

			if (!isBiomeAllowed(settings.biomes, getBiomeAt(terrainData, targetX))) {
				// stop when crossing biomes
				break;
			}

			if (isValidOreBlock(settings, targetX, targetY)) {
				m_GameMap.blockUnsafe(targetX, targetY).type = settings.type;
			}

			spawnPos.x += getRandomFloat(rng, -1.f, 1.f);
			spawnPos.y += getRandomFloat(rng, -1.f, 1.f);
		}
	};

	for (const OreSettings& settings : m_Settings.ores) {
		std::vector<int> validX {};

		for (const Biome& biome : terrainData.biomes) {
			if (!isBiomeAllowed(settings.biomes, &biome)) {
				continue;
			}

			for (int x { biome.startX }; x < biome.endX; x++) {
				validX.push_back(x);
			}
		}

		if (validX.empty()) {
			// no fitting biome found
			continue;
		}

		for (int i {}; i < settings.veinCount; i++) {
			std::optional<Vector2> spawnPos {};

			// retry until spawn position is found
			constexpr int maxRetries { 100 };
			for (int i {}; i < maxRetries; i++) {
				int x { validX[getRandomInt(rng, 0, validX.size() - 1)] };
				int y { getRandomInt(rng, settings.minDepth, settings.maxDepth) };

				if (!isValidOreBlock(settings, x, y)) {
					continue;
				}

				spawnPos = {
					static_cast<float>(x),
					static_cast<float>(y),
				};
				break;
			}

			if (!spawnPos.has_value()) {
				// no spawn position found
				break;
			}

			generateVein(settings, spawnPos.value());
		}
	}
}

NoiseData WorldGenerator::generateNoiseData() {
	auto generateNoise
	    = [&](const NoiseSettings& noiseSettings, bool twoDimensions = false) {
		      auto simplex { FastNoise::New<FastNoise::Simplex>() };
		      auto fractal { FastNoise::New<FastNoise::FractalFBm>() };

		      fractal->SetSource(simplex);
		      fractal->SetOctaveCount(noiseSettings.octaves);
		      simplex->SetScale(1.f / noiseSettings.frequency);

		      std::vector<float> noise {};

		      if (!twoDimensions) {
			      noise.resize(WIDTH);
			      fractal->GenUniformGrid2D(
			          noise.data(), 0.f, 0.f, WIDTH, 1, 1.f, 1.f, m_Seed++);
		      } else {
			      noise.resize(WIDTH * HEIGHT);
			      fractal->GenUniformGrid2D(
			          noise.data(), 0.f, 0.f, WIDTH, HEIGHT, 1.f, 1.f, m_Seed++);
		      }

		      // convert from [-1 1] to [0 1]
		      for (float& value : noise) {
			      value = (value + 1.f) / 2.f;
		      }

		      return std::move(noise);
	      };

	std::vector<float> mountain { generateNoise(m_Settings.mountain) };
	std::vector<float> plains { generateNoise(m_Settings.plains) };
	std::vector<float> blend { generateNoise(m_Settings.blend) };
	std::vector<float> stone { generateNoise(m_Settings.stone) };
	std::vector<float> cave1 { generateNoise(m_Settings.cave1, true) };
	std::vector<float> cave2 { generateNoise(m_Settings.cave2, true) };

	return { std::move(mountain), std::move(plains), std::move(blend),
		std::move(stone), std::move(cave1), std::move(cave2) };
}
