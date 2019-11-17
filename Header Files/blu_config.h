#pragma once
#include "pch.h"


constexpr unsigned APP_MAJOR_VERSION	 = 0;
constexpr unsigned APP_MINOR_VERSION	 = 1;
constexpr unsigned APP_BLU_PATCH_VERSION = 0;

constexpr unsigned ENG_MAJOR_VERSION	 = 0;
constexpr unsigned ENG_MINOR_VERSION	 = 1;
constexpr unsigned ENG_BLU_PATCH_VERSION = 0;

//#define IMGUI_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
#define IMGUI_VULKAN_DEBUG_REPORT
#endif
