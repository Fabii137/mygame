#pragma once

#include <cstdint>

#include <limits>
#include <memory>
#include <type_traits>
#include <unordered_map>

#include "asserts.h"
#include "entity.hpp"

constexpr std::uint64_t PLAYER_ID { 1 };

struct EntityIdHolder {
	std::uint64_t idCounter { 2 };

	std::uint64_t getIdAndIncrement() {
		std::uint64_t id { idCounter++ };
		permaAssertComment(id < std::numeric_limits<std::uint64_t>::max() - 1,
		    "We ran out of ids somehow...");

		return id;
	}
};

struct EntityHolder {
	EntityIdHolder idHolder {};
	std::unordered_map<std::uint64_t, std::unique_ptr<Entity>> entities;

	template <typename T>
	  requires std::is_base_of_v<Entity, T>
	void add(T entity) {
		std::uint64_t id { idHolder.getIdAndIncrement() };
		entities[id] = std::make_unique<T>(std::move(entity));
	}
};
