#pragma once
#include <cstdint>

#include <random>
#include <vector>

#include "blocks.hpp"
#include "gameMap.hpp"

struct NoiseData {
  float *mountain{};
  float *plains{};
  float *blend{};
  float *stone{};
  float *cave1{};
  float *cave2{};

  NoiseData(float *mountain, float *plains, float *blend, float *stone,
            float *cave1, float *cave2);
  ~NoiseData();

  NoiseData(const NoiseData &) = delete;
  NoiseData &operator=(const NoiseData &) = delete;
};

struct Biome {
  enum Type : std::uint16_t {
    Forest = 0,
    Desert,
    Snow,
    BIOMES_COUNT,
  };

  Type type{};
  int startX{};
  int endX{};
};

struct TerrainData {
  std::vector<int> surfaceHeights{};
  std::vector<int> stoneHeights{};
  std::vector<Biome> biomes{};
};

struct NoiseSettings {
  float frequency{0.02f};
  int octaves{1};
};

struct OreSettings {
  Block::Type type{};
  int veinCount{};
  int minVeinSize{};
  int maxVeinSize{};
  int minDepth{};
  int maxDepth{};
  std::vector<Biome::Type> biomes{};
  std::vector<Block::Type> canReplace{};
};

struct StructureSettings {
  // names of structure variants
  std::vector<std::string> variants;
  std::vector<Biome::Type> biomes{};

  float spawnChance{};
};

struct WorldSettings {
  std::vector<OreSettings> ores{
      {
          .type = Block::Copper,
          .veinCount = 150,
          .minVeinSize = 3,
          .maxVeinSize = 8,
          .minDepth = 220,
          .maxDepth = 450,
      },
      {
          .type = Block::Iron,
          .veinCount = 80,
          .minVeinSize = 3,
          .maxVeinSize = 7,
          .minDepth = 260,
          .maxDepth = 450,
      },
      {
          .type = Block::Gold,
          .veinCount = 40,
          .minVeinSize = 2,
          .maxVeinSize = 5,
          .minDepth = 300,
          .maxDepth = 450,
      },
      {
          .type = Block::SandRuby,
          .veinCount = 40,
          .minVeinSize = 2,
          .maxVeinSize = 5,
          .minDepth = 220,
          .maxDepth = 400,
          .biomes = {Biome::Desert},
          .canReplace = {Block::Sand, Block::SandStone},
      },
      {
          .type = Block::SnowBlueRuby,
          .veinCount = 40,
          .minVeinSize = 2,
          .maxVeinSize = 5,
          .minDepth = 220,
          .maxDepth = 400,
          .biomes = {Biome::Snow},
          .canReplace = {Block::Snow},
      },
  };

  std::vector<StructureSettings> structures{
      {
          .variants = {"tree", "tree2", "tree3"},
          .biomes = {Biome::Forest},
          .spawnChance = 0.08f,
      },
      {
          .variants = {"house"},
          .biomes = {Biome::Forest},
          .spawnChance = 0.005f,
      },
      {
          .variants = {"pyramid"},
          .biomes = {Biome::Desert},
          .spawnChance = 0.005f,
      },
      {
          .variants = {"igloo"},
          .biomes = {Biome::Snow},
          .spawnChance = 0.005f,
      },
  };

  NoiseSettings mountain{
      .frequency = 0.003f,
      .octaves = 4,
  };

  NoiseSettings plains{
      .frequency = 0.02f,
      .octaves = 1,
  };

  NoiseSettings blend{
      .frequency = 0.003f,
      .octaves = 1,
  };

  NoiseSettings stone{
      .frequency = 0.005f,
      .octaves = 3,
  };

  NoiseSettings cave1{
      .frequency = 0.015f,
      .octaves = 2,
  };

  NoiseSettings cave2{
      .frequency = 0.04f,
      .octaves = 3,
  };

  int mountainHeightStart{80};
  int mountainHeightEnd{130};

  int plainsHeightStart{140};
  int plainsHeightEnd{170};

  // offset to dirt
  int stoneOffsetStart{-10};
  int stoneOffsetEnd{50};

  float minCaveNoise{0.85f};
};

class WorldGenerator {
public:
  WorldGenerator(GameMap &gameMap, const WorldSettings &settings,
                 std::uint32_t seed);

  void generate();

private:
  TerrainData generateBaseTerrain(const NoiseData &noiseData);
  void generateBiomeMap(TerrainData &terrainData, std::ranlux24_base &rng);
  const Biome *getBiomeAt(const TerrainData &terrainData, int x) const;
  bool isBiomeAllowed(const std::vector<Biome::Type> &allowed,
                      const Biome *biome);
  bool isValidSurfaceBlock(const Biome &biome, int x, int y);
  void applyBiomes(const TerrainData &terrainData);
  void generateDesertColumn(int x, const Biome &biome,
                            const TerrainData &terrainData);
  void generateForestColumn(int x, const Biome &biome,
                            const TerrainData &terrainData);
  void generateSnowColumn(int x, const Biome &biome,
                          const TerrainData &terrainData);
  void generateCaves(const NoiseData &noiseData);
  void generateWorms(std::ranlux24_base &rng);
  void generateStructures(const TerrainData &terrainData,
                          std::ranlux24_base &rng);
  void generateOres(const TerrainData &terrainData, std::ranlux24_base &rng);
  NoiseData generateNoiseData();

private:
  static constexpr int WIDTH{1200};
  static constexpr int HEIGHT{500};

  GameMap &m_GameMap;
  const WorldSettings &m_Settings;
  std::uint32_t m_Seed;
};
