#pragma once
#include "pch.h"

class GameContext{
public:
	void main_loop();
	void process_event(SDL_Event);
private:
	//ComponentStorage c;
};
