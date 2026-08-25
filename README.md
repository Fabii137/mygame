# mygame

A small sandbox terrain game built in C++ with raylib and ImGui.

## You can:

- walk around a generated map
- break and place blocks
- tweak world generation settings and creative tools via the debug UI

## Controls

- AD / arrow left/right: move left/right
- WS / arrow up/down: fly up/down in creative mode
- Space: jump
- Left click: mine / attack
- Right click: place block
- Middle click: pick block type in creative mode
- F10: toggle debug/creative tools

## Run it

From the project root:

```bash
cmake -S . -B build
cmake --build build
./build/mygame
```

## Notes

This is currently more of a toy sandbox prototype than a finished game.

## Tech

- C++23
- [raylib](https://github.com/raysan5/raylib)
- [imGui](https://github.com/ocornut/imgui)
- [rlImGui](https://github.com/raylib-extras/rlImGui)
- [FastNoise2](https://github.com/Auburn/FastNoise2)
- [nlohmann/json](https://github.com/nlohmann/json)
