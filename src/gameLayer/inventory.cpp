#include "inventory.hpp"

#include "asserts.h"
#include "assetManager.hpp"
#include "helpers.hpp"
#include "ui.hpp"

Inventory::Inventory(size_t slots, size_t cols)
    : m_Cols(cols) {
	permaAssertCommentDevelopment(m_Cols > 0, "Inventory can't have 0 columns");

	m_Items.resize(slots);

	for (size_t i {}; i < slots; i++) {
		std::uint16_t type { static_cast<std::uint16_t>(i + 1) };
		size_t count { static_cast<size_t>(((i + 1) * 20) % maxStackSize(type)) };
		m_Items[i].type = static_cast<Item::Type>(type);
		m_Items[i].count = count;
	}
}

int Inventory::add(Item items) {
	if (items.empty()) {
		return items.count;
	}

	// try non-empty stacks
	for (Item& stack : m_Items) {
		if (stack.type != items.type) {
			continue;
		}

		size_t total { stack.count + items.count };
		stack.count = std::min(total, maxStackSize(stack.type));
		items.count = total - stack.count;
		if (items.count == 0) {
			return 0;
		}
	}

	// try empty slots
	for (Item& stack : m_Items) {
		if (!stack.empty()) {
			continue;
		}

		stack.type = items.type;
		stack.count = std::min(items.count, maxStackSize(items.type));
		items.count -= stack.count;
		if (items.count == 0) {
			return 0;
		}
	}

	return items.count;
}

std::optional<size_t> Inventory::hoveredSlot(Rectangle bounds) const {
	Vector2 mousePos { GetMousePosition() };
	std::optional<size_t> result {};

	forEachCell(bounds, [&](size_t i, size_t j, Rectangle cell) {
		if (CheckCollisionPointRec(mousePos, cell)) {
			result = i * m_Cols + j;
		}
	});
	return result;
}

bool Inventory::move(size_t from, size_t to) {
	if (from >= m_Items.size() || to >= m_Items.size()) {
		return false;
	}

	std::swap(m_Items[from], m_Items[to]);
	return true;
}

Item Inventory::take(size_t slot, size_t count) {
	if (slot >= m_Items.size() || count <= 0) {
		return {};
	}

	Item& stored { m_Items[slot] };
	if (stored.empty()) {
		return {};
	}

	size_t takenCount { std::min(count, stored.count) };
	Item result { stored.type, takenCount };
	stored.count -= result.count;
	if (stored.count == 0) {
		stored.type = {};
	}
	return result;
}

Item Inventory::insert(size_t slot, Item stack) {
	if (slot >= m_Items.size() || stack.empty()) {
		return stack;
	}

	Item& stored { m_Items[slot] };
	if (!stored.empty() && stored.type != stack.type) {
		return stack;
	}

	if (stored.empty()) {
		stored.type = stack.type;
	}

	size_t available { maxStackSize(stored.type) - stored.count };
	size_t inserted { std::min(available, stack.count) };
	stored.count += inserted;
	stack.count -= inserted;
	if (stack.count == 0) {
		stack.type = {};
	}
	return stack;
}

void Inventory::render(
    const AssetManager& assetManager, Rectangle bounds) const {
	constexpr float padding { 0.1f };
	Vector2 screenSize { getScreenSize() };

	DrawRectangleRec(bounds, { 100, 100, 100, 100 });

	forEachCell(bounds, [&](size_t i, size_t j, Rectangle cell) {
		Rectangle src { 0.f, 0.f, static_cast<float>(assetManager.frame.width),
			static_cast<float>(assetManager.frame.height) };
		if (CheckCollisionPointRec(GetMousePosition(), cell)) {
			drawTexture(assetManager.frame, src, cell, { 220, 250, 220, 250 });
		} else {
			drawTexture(assetManager.frame, src, cell, { 180, 180, 200, 240 });
		}

		const Item& stack { m_Items[i * m_Cols + j] };
		renderItemStack(assetManager, cell, stack);
	});
}

size_t Inventory::size() const { return m_Items.size(); }

Item& Inventory::slot(size_t index) { return m_Items.at(index); }

const Item& Inventory::slot(size_t index) const { return m_Items.at(index); }

void Inventory::forEachCell(Rectangle bounds,
    std::function<void(size_t, size_t, Rectangle)> func) const {
	size_t rows { this->rows() };

	bounds = UI::shrinkRectPercentage(bounds, PADDING, PADDING);

	Rectangle cellRect {};
	cellRect.height = bounds.height / rows;
	cellRect.width = cellRect.height;
	cellRect.x = bounds.x;
	cellRect.y = bounds.y;

	for (size_t i {}; i < rows; i++) {
		size_t slotsLeft { m_Items.size() - i * m_Cols };
		size_t cols { std::min(slotsLeft, m_Cols) };
		for (size_t j {}; j < cols; j++) {
			Rectangle cell { cellRect };
			cell.y += i * cellRect.height;
			cell.x += j * cellRect.width;
			cell = UI::shrinkRectPercentage(cell, PADDING, PADDING);
			func(i, j, cell);
		}
	}
}

size_t Inventory::rows() const {
	return (m_Items.size() + m_Cols - 1) / m_Cols;
}

void Inventory::renderItemStack(const AssetManager& assetManager,
    const Rectangle& cellRect, const Item& stack) const {
	if (!stack.type) {
		return;
	}
	Texture2D texture { getTextureForItemType(stack.type, assetManager) };
	Rectangle source { getTextureCoordsForItemType(stack.type) };

	Rectangle itemRect { UI::shrinkRectPercentage(cellRect, PADDING, PADDING) };
	drawTexture(texture, source, itemRect);

	if (stack.count > 1) {
		int textX { static_cast<int>(cellRect.x + cellRect.width * 0.6f) };
		int textY { static_cast<int>(cellRect.y + cellRect.height * 0.7f) };
		DrawText(TextFormat("%u", stack.count), textX, textY, 20, WHITE);
	}
}
