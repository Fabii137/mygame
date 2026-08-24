cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(PRODUCTION_BUILD OFF CACHE BOOL "Make this a production build" FORCE)

# only x64/x86 supports AVX2/FMA optimizations
set(ENABLE_AVX2 OFF CACHE BOOL "Enable AVX2/FMA optimizations for x86/x64 builds")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|i[3-6]86)$")
  set(ENABLE_AVX2 ON CACHE BOOL "Enable AVX2/FMA optimizations for x86/x64 builds" FORCE)
endif()

if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  if(ENABLE_AVX2)
    add_compile_options(/arch:AVX2) # simd optimizations
  endif()
elseif(APPLE)
  # no static libstdc++/libgcc on macOS
  if(ENABLE_AVX2)
    add_compile_options(-mavx2 -mfma) # simd optimizations
  endif()
else()
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND (MINGW OR CMAKE_SYSTEM_NAME STREQUAL "Linux"))
    add_link_options(-static-libstdc++ -static-libgcc)
  endif()
  if(ENABLE_AVX2)
    add_compile_options(-mavx2 -mfma) # simd optimizations
  endif()
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build libraries statically" FORCE)

project(mygame)

add_subdirectory(thirdparty/raylib-6.0)
add_subdirectory(thirdparty/imgui-1.89.7-docking)
add_subdirectory(thirdparty/rlimgui)
add_subdirectory(thirdparty/FastNoise2-1.1.1)
add_subdirectory(thirdparty/json-3.12.0)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable("${CMAKE_PROJECT_NAME}")
set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY CXX_STANDARD 23)

if(PRODUCTION_BUILD)
  target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="./resources/")
  target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=1)
  target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC DEVELOPMENT_BUILD=0)
else()
  target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
  target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=0)
  target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC DEVELOPMENT_BUILD=1)
endif()

target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_SOURCES})

if(MSVC)
  target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC _CRT_SECURE_NO_WARNINGS)

  if(PRODUCTION_BUILD)
    set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") # no console
  endif()

elseif(MINGW)
  if(PRODUCTION_BUILD)
    # no console
    target_link_options("${CMAKE_PROJECT_NAME}" PRIVATE
      -mwindows
    )
  endif()

elseif(APPLE OR UNIX)
  if(PRODUCTION_BUILD)
    target_link_options("${CMAKE_PROJECT_NAME}" PRIVATE -s)
  endif()
endif()

target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/gameLayer")
target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/platform")
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE raylib_static imgui rlimgui FastNoise2 nlohmann_json)

