#include "game.hpp"
#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"

int main() {
#if PRODUCTION_BUILD == 1
	SetTraceLogLevel(LOG_NONE);
#endif

	ChangeDirectory(GetApplicationDirectory());

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(800, 450, "mygame");
	SetExitKey(KEY_NULL);
	SetTargetFPS(240);

	rlImGuiSetup(true);
	ImGuiIO& io { ImGui::GetIO() };
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.FontGlobalScale = 1.f;

	Game game {};
	if (!game.init()) {
		return 0;
	}

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);

		rlImGuiBegin();
		ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
		ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, {});
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		ImGui::PopStyleColor(2);

		bool shouldExit { !game.update() };

		rlImGuiEnd();
		EndDrawing();

		if (shouldExit) {
			break;
		}
	}

	rlImGuiShutdown();
	CloseWindow();
	game.close();

	return 0;
}
