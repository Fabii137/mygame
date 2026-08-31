#include "inventoryController.hpp"

void InventoryController::update(InventoryContext& ctx) {
	bool mouseLeftPressed { IsMouseButtonPressed(MOUSE_BUTTON_LEFT) };
	bool mouseRightPressed { IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) };

	bool dropAction { mouseLeftPressed || mouseRightPressed };

	if (m_DragState.active && dropAction) {
		if (auto slot { hoveredSlot(ctx) }) {
			drop(ctx, slot.value(), mouseRightPressed);
		}
	} else if (mouseLeftPressed) {
		if (auto slot { hoveredSlot(ctx) }) {
			beginDrag(ctx, slot.value());
		}
	}
}

void InventoryController::update(
    InventoryContext& ctx, InventoryContext& otherCtx) {
	bool mouseLeftPressed { IsMouseButtonPressed(MOUSE_BUTTON_LEFT) };
	bool mouseRightPressed { IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) };

	bool dropAction { mouseLeftPressed || mouseRightPressed };

	if (m_DragState.active && dropAction) {
		if (auto slot { hoveredSlot(ctx) }) {
			drop(ctx, slot.value(), mouseRightPressed);
			return;
		}

		if (auto slot { hoveredSlot(otherCtx) }) {
			drop(otherCtx, slot.value(), mouseRightPressed);
		}
	} else if (mouseLeftPressed) {
		if (auto slot { hoveredSlot(ctx) }) {
			beginDrag(ctx, slot.value());
			return;
		}

		if (auto slot { hoveredSlot(otherCtx) }) {
			beginDrag(otherCtx, slot.value());
		}
	}
}

void InventoryController::render(const AssetManager& assetManager) {
	if (!m_DragState.active) {
		return;
	}

	Vector2 mousePos { GetMousePosition() };
	Rectangle ghostRect {
		mousePos.x - m_DragState.ghostSize * 0.5f,
		mousePos.y - m_DragState.ghostSize * 0.5f,
		m_DragState.ghostSize,
		m_DragState.ghostSize,
	};

	drawItemStack(assetManager, ghostRect, m_DragState.draggedItem);
}

void InventoryController::onInventoryClosed() {
	if (!m_DragState.active) {
		return;
	}

	Inventory* src = m_DragState.sourceInventory;
	if (src) {
		Item& sourceSlot = src->slot(m_DragState.sourceSlot);

		if (sourceSlot.empty()) {
			sourceSlot = m_DragState.draggedItem;
		} else {
			size_t leftover { src->add(m_DragState.draggedItem) };
			if (leftover > 0) {
				// TODO: could not add; drop items
			}
		}
	}

	cancelDrag();
}

std::optional<size_t> InventoryController::hoveredSlot(
    const InventoryContext& ctx) {
	return ctx.inventory.hoveredSlot(ctx.bounds);
}

void InventoryController::beginDrag(InventoryContext& ctx, size_t slot) {
	Item& item = ctx.inventory.slot(slot);
	if (item.empty()) {
		return;
	}

	m_DragState.active = true;
	m_DragState.sourceInventory = &ctx.inventory;
	m_DragState.sourceSlot = slot;
	m_DragState.draggedItem = item;
	m_DragState.ghostSize = ctx.inventory.cellSize(ctx.bounds);

	item.type = {};
	item.count = 0;
}

void InventoryController::drop(
    InventoryContext& targetCtx, size_t targetSlot, bool single) {
	if (!m_DragState.active)
		return;

	Inventory* src = m_DragState.sourceInventory;
	if (!src) {
		cancelDrag();
		return;
	}

	m_DragState.draggedItem
	    = targetCtx.inventory.insert(targetSlot, m_DragState.draggedItem, single);
	if (m_DragState.draggedItem.empty()) {
		cancelDrag();
	}
}

void InventoryController::cancelDrag() { m_DragState = {}; }
