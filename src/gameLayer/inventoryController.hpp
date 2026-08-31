#pragma once

#include <optional>

#include "helpers.hpp"
#include "inventory.hpp"
#include "items.hpp"
#include "raylib.h"

struct DragState {
	bool active {};
	Inventory* sourceInventory {};
	size_t sourceSlot {};
	Item draggedItem {};
	float ghostSize {};
};

struct InventoryContext {
	Inventory& inventory;
	const Rectangle& bounds;
};

class InventoryController {
public:
	void update(InventoryContext& ctx);
	void update(InventoryContext& ctx, InventoryContext& otherCtx);

	void render(const AssetManager& assetManager);

	void onInventoryClosed();

private:
	std::optional<size_t> hoveredSlot(const InventoryContext& ctx);

	void beginDrag(InventoryContext& ctx, size_t slot);

	// single = only drop one item
	void drop(
	    InventoryContext& targetCtx, size_t targetSlot, bool single = false);

	void cancelDrag();

private:
	DragState m_DragState {};
};
