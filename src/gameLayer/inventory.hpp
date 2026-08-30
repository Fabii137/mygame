#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <vector>

#include "items.hpp"
#include "raylib.h"

struct AssetManager;

class Inventory {
public:
	Inventory(size_t slots, size_t cols);

	int add(Item items);
	bool move(size_t from, size_t to);
	Item take(size_t slot, size_t count);
	Item insert(size_t slot, Item stack);

	void render(const AssetManager& assetManager, Rectangle bounds) const;

	size_t size() const;

	Item& slot(size_t index);
	const Item& slot(size_t index) const;

	std::optional<size_t> hoveredSlot(Rectangle bounds) const;

private:
	void forEachCell(Rectangle bounds,
	    std::function<void(size_t, size_t, Rectangle)> func) const;

	size_t rows() const;

	void renderItemStack(const AssetManager& assetManager,
	    const Rectangle& cellRect, const Item& stack) const;

private:
	static constexpr float PADDING { 0.1f };
	std::vector<Item> m_Items {};
	size_t m_Cols {};
};
