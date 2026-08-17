#include <cstddef>
#include <fstream>

#include "saveMap.hpp"

#include "asserts.h"
#include "blocks.hpp"
#include "walls.hpp"

struct BlockSaveRepresentation1 {
  Block::Type type{};

  Block toBlock() const {
    return {
        .type = type,
    };
  }
};

struct WallSaveRepresentation1 {
  Wall::Type type{};

  Wall toWall() const {
    return {
        .type = type,
    };
  }
};

constexpr int VERSION{1};

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

bool saveMapDataToFile(const std::vector<Block> &blocks,
                       const std::vector<Wall> &walls, int w, int h,
                       const char *fileName) {

  std::ofstream f{fileName, std::ios::binary};
  if (!f.is_open()) {
    return false;
  }

  size_t totalCount{static_cast<size_t>(w) * static_cast<size_t>(h)};
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

  f.write(reinterpret_cast<const char *>(&VERSION), sizeof(VERSION));

  f.write(reinterpret_cast<const char *>(&w), sizeof(w));
  f.write(reinterpret_cast<const char *>(&h), sizeof(h));

  for (size_t i{}; i < totalCount; i++) {
    auto b{toBlockRepresentation(blocks[i])};
    f.write(reinterpret_cast<const char *>(&b), sizeof(b));

    auto w{toWallRepresentation(walls[i])};
    f.write(reinterpret_cast<const char *>(&w), sizeof(w));
  }

  return f.good();
}

bool loadMapDataFromFile(std::vector<Block> &blocks, std::vector<Wall> &walls,
                         int &w, int &h, const char *fileName) {
  blocks.clear();
  walls.clear();
  w = 0;
  h = 0;

  std::ifstream f{fileName, std::ios::binary};
  if (!f.is_open()) {
    return false;
  }

  int version{};
  f.read(reinterpret_cast<char *>(&version), sizeof(version));

  f.read(reinterpret_cast<char *>(&w), sizeof(w));
  f.read(reinterpret_cast<char *>(&h), sizeof(h));
  if (!f || w <= 0 || h <= 0) {
    return false;
  }

  if (w > 10000 || h > 10000) {
    // probably corrupt data
    return false;
  }

  size_t totalCount{static_cast<size_t>(w) * static_cast<size_t>(h)};
  switch (version) {
  case 1: {
    blocks.resize(totalCount);
    walls.resize(totalCount);

    for (size_t i{}; i < totalCount; i++) {
      BlockSaveRepresentation1 readBlock{};
      WallSaveRepresentation1 readWall{};

      f.read(reinterpret_cast<char *>(&readBlock), sizeof(readBlock));
      f.read(reinterpret_cast<char *>(&readWall), sizeof(readWall));

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

  for (size_t i{}; i < totalCount; i++) {
    blocks[i].sanitize();
    walls[i].sanitize();
  }

  return true;
}
