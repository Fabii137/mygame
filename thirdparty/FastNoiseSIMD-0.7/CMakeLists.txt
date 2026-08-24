
cmake_minimum_required(VERSION 3.16)
project(fastNoiseSIMD)

set(FN_X86 FALSE)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|i[3-6]86)$")
  set(FN_X86 TRUE)
endif()

file(GLOB_RECURSE FN_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/FastNoiseSIMD/*.cpp"
)

add_library(fastNoiseSIMD)

foreach(SRC IN LISTS FN_SOURCES)
  if(SRC MATCHES "FastNoiseSIMD_sse2\\.cpp$" AND NOT FN_X86)
    list(REMOVE_ITEM FN_SOURCES "${SRC}")
  elseif(SRC MATCHES "FastNoiseSIMD_sse41\\.cpp$" AND NOT FN_X86)
    list(REMOVE_ITEM FN_SOURCES "${SRC}")
  elseif(SRC MATCHES "FastNoiseSIMD_avx2\\.cpp$" AND NOT FN_X86)
    list(REMOVE_ITEM FN_SOURCES "${SRC}")
  elseif(SRC MATCHES "FastNoiseSIMD_avx512\\.cpp$" AND NOT FN_X86)
    list(REMOVE_ITEM FN_SOURCES "${SRC}")
  endif()
endforeach()

target_sources(fastNoiseSIMD PRIVATE ${FN_SOURCES})

if(FN_X86)
  if(MSVC)
    set_source_files_properties(
      "${CMAKE_CURRENT_SOURCE_DIR}/FastNoiseSIMD/FastNoiseSIMD_sse2.cpp"
      PROPERTIES COMPILE_FLAGS "/arch:SSE2"
    )
    set_source_files_properties(
      "${CMAKE_CURRENT_SOURCE_DIR}/FastNoiseSIMD/FastNoiseSIMD_sse41.cpp"
      PROPERTIES COMPILE_FLAGS "/arch:SSE2"
    )
    set_source_files_properties(
      "${CMAKE_CURRENT_SOURCE_DIR}/FastNoiseSIMD/FastNoiseSIMD_avx2.cpp"
      PROPERTIES COMPILE_FLAGS "/arch:AVX2"
    )
  else()
    set_source_files_properties(
      "${CMAKE_CURRENT_SOURCE_DIR}/FastNoiseSIMD/FastNoiseSIMD_sse2.cpp"
      PROPERTIES COMPILE_FLAGS "-msse2"
    )
    set_source_files_properties(
      "${CMAKE_CURRENT_SOURCE_DIR}/FastNoiseSIMD/FastNoiseSIMD_sse41.cpp"
      PROPERTIES COMPILE_FLAGS "-msse4.1"
    )
    set_source_files_properties(
      "${CMAKE_CURRENT_SOURCE_DIR}/FastNoiseSIMD/FastNoiseSIMD_avx2.cpp"
      PROPERTIES COMPILE_FLAGS "-mavx2 -mfma"
    )
  endif()
endif()
