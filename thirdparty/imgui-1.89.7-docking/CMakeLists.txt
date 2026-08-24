cmake_minimum_required(VERSION 3.16)
project(imgui)

add_library(imgui)
target_sources(imgui PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(imgui PRIVATE -Wno-nontrivial-memcall)
elseif(MSVC)
  target_compile_options(imgui PRIVATE /wd4996)
endif()


target_include_directories(imgui SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
