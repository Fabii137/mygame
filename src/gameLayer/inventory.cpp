#include <string>
#include <utility>

#include "inventory.hpp"

#include "asserts.h"
#include "assetManager.hpp"
#include "helpers.hpp"
#include "raylib.h"
#include "ui.hpp"

#include "nlohmann/json.hpp"

Inventory::Inventory(size_t slots, size_t cols)
    : m_Cols(cols) {
	permaAssertCommentDevelopment(m_Cols > 0, "Inventory can't have 0 columns");

	m_Items.resize(slots);

	// for (size_t i {}; i < slots; i += 2) {
	// 	std::uint16_t type { static_cast<std::uint16_t>(i + 1) };
	// 	size_t count { static_cast<size_t>(((i + 1) * 20) % maxStackSize(type)) };
	// 	m_Items[i].type = static_cast<Item::Type>(type);
	// 	m_Items[i].count = count;
	// }
}

size_t Inventory::add(Item items) {
	if (items.empty()) {
		return items.count;
	}

	// try non-empty slots
	for (Item& stack : m_Items) {
		if (stack.type != items.type) {
			continue;
		}

		size_t total { stack.count + items.count };
		stack.count = std::min(total, stack.maxStackSize());
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
		stack.count = std::min(items.count, items.maxStackSize());
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

	forEachCell(bounds, [&](size_t i, size_t j, const Rectangle& cell) {
		if (CheckCollisionPointRec(mousePos, cell)) {
			result = i * m_Cols + j;
		}
	});
	return result;
}

Item Inventory::insert(size_t slot, Item stack, bool single) {
	if (slot >= m_Items.size() || stack.empty()) {
		return stack;
	}

	Item& stored { m_Items[slot] };
	if (!stored.empty() && stored.type != stack.type) {
		if (!single) {
			// swap items if using the whole stack
			std::swap(stored, stack);
		}
		return stack;
	}

	if (stored.empty()) {
		stored.type = stack.type;
	}

	size_t count { single ? 1 : stack.count };

	size_t available { stored.maxStackSize() - stored.count };
	size_t inserted { std::min(available, count) };
	stored.count += inserted;
	stack.count -= inserted;
	if (stack.empty()) {
		stack.type = {};
	}
	return stack;
}

void Inventory::render(
    const AssetManager& assetManager, Rectangle bounds) const {

	std::optional<size_t> hoveredIdx {};
	forEachCell(bounds, [&](size_t i, size_t j, const Rectangle& cell) {
		Rectangle src { 0.f, 0.f, static_cast<float>(assetManager.frame.width),
			static_cast<float>(assetManager.frame.height) };
		size_t idx { i * m_Cols + j };
		const Item& stack { m_Items[idx] };

		bool hovered { CheckCollisionPointRec(GetMousePosition(), cell) };
		if (hovered) {
			drawTexture(assetManager.frame, src, cell, { 220, 250, 220, 250 });
			hoveredIdx = idx;
		} else {
			drawTexture(assetManager.frame, src, cell, { 180, 180, 200, 240 });
		}

		drawItemStack(assetManager, cell, stack);
	});

	if (hoveredIdx.has_value()) {
		renderItemTooltip(hoveredIdx.value());
	}
}

void Inventory::renderItemTooltip(size_t idx) const {
	const Item& stack { this->slot(idx) };
	if (stack.empty()) {
		return;
	}

	bool hasCount { stack.count > 1 };

	constexpr int fontSize { 18 };
	const std::string& name { stack.name() };
	const std::string countStr { hasCount ? " x" + std::to_string(stack.count)
		                                    : "" };
	// TODO: add description

	int nameWidth { MeasureText(name.c_str(), fontSize) };
	int countWidth { countStr.empty() ? 0
		                                : MeasureText(countStr.c_str(), fontSize) };
	int textWidth { nameWidth + countWidth };

	constexpr int tooltipPaddingX = 12;
	constexpr int tooltipPaddingY = 8;
	int width { textWidth + tooltipPaddingX * 2 };
	int height { fontSize + tooltipPaddingY * 2 };

	Vector2 mousePos = GetMousePosition();
	Vector2 pos { mousePos.x + 16.f, mousePos.y - 16.f };
	Vector2 screenSize { getScreenSize() };
	if (pos.x + width > screenSize.x) {
		pos.x = mousePos.x - width - 16.f;
	}
	if (pos.y < 0) {
		pos.y = mousePos.y + height + 16.f;
	}

	Rectangle bounds {
		pos.x,
		pos.y,
		static_cast<float>(width),
		static_cast<float>(height),
	};
	DrawRectangleRounded(bounds, 0.25f, 8, { 0, 0, 0, 200 });
	DrawRectangleRoundedLinesEx(bounds, 0.25f, 9, 1.f, { 80, 80, 90, 220 });

	float textX { pos.x + tooltipPaddingX };
	float textY { pos.y + tooltipPaddingY };
	DrawText(name.c_str(), textX, textY, fontSize, WHITE);
	if (!countStr.empty()) {
		DrawText(countStr.c_str(), textX + nameWidth, textY, fontSize,
		    { 200, 200, 255, 255 });
	}
}

Json Inventory::formatToJson() const {
	Json json {};

	Json itemsJson = Json::array();
	for (const Item& item : m_Items) {
		itemsJson.push_back(item.formatToJson());
	}
	json["items"] = itemsJson;

	return json;
}

bool Inventory::loadFromJson(Json& json) {
	m_Items.clear();

	if (!json.is_object()) {
		return false;
	}

	if (!json.contains("items") || !json["items"].is_array()) {
		return false;
	}

	for (Json& itemJson : json["items"]) {
		Item item {};

		if (!item.loadFromJson(itemJson)) {
			m_Items.push_back({});
			continue;
		}

		m_Items.push_back(item);
	}

	return true;
}

float Inventory::cellSize(Rectangle bounds) const {
	bounds = UI::shrinkRectPercentage(bounds, PADDING, PADDING);

	Rectangle cellRect {};
	cellRect.height = bounds.height / rows();
	cellRect.width = cellRect.height;
	cellRect.x = 0.f;
	cellRect.y = 0.f;

	cellRect = UI::shrinkRectPercentage(cellRect, PADDING, PADDING);
	return cellRect.width;
}

float Inventory::aspectRatio() const {
	if (m_Items.empty()) {
		return 1.f;
	}

	return static_cast<float>(m_Cols) / static_cast<float>(rows());
}

size_t Inventory::size() const { return m_Items.size(); }

Item& Inventory::slot(size_t index) { return m_Items.at(index); }

const Item& Inventory::slot(size_t index) const { return m_Items.at(index); }

void Inventory::forEachCell(Rectangle bounds, CellFunc func) const {
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
