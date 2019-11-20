#pragma once

#include "pch.h"


struct UserInfo {
	u8 fullscreen;
	u32 win_height;
	u32 win_width;

	static const int size = 9; // u8+u32+u32 /8
};
