#include "pch.h"

#include "load_save.h"

user_info load_user_info() {
	user_info info;
	// open config file.
	// read user settings and fill user_info struct
  // for now leaving this as manually fed values.
	info.fullscreen = false; //etc
	info.win_height = 720;
	info.win_width = 1250;
	return info;
}

void store_user_info(const user_info& info) {
	// open config file
	//write user settings and save
}
