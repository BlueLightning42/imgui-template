#pragma once

#include "pch.h"

// This whole file is basically one big old TODO

struct user_info {
	bool fullscreen;
	int win_height;
	int win_width;
};

user_info load_user_info();

void store_user_info(const user_info&);
