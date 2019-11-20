#include "pch.h"

#include "graphics.h"
#include "game_manager.h"

void GraphicContext::process_event(SDL_Event event) {
	if (			event.type == SDL_WINDOWEVENT &&
					event.window.event == SDL_WINDOWEVENT_RESIZED &&
					event.window.windowID == SDL_GetWindowID(gui.sdl_window) ) {
		v._SwapChainResizeWidth = (int)event.window.data1;
		v._SwapChainResizeHeight = (int)event.window.data2;
		v._SwapChainRebuild = true;
	}
}

void GameContext::process_event(SDL_Event event) {

}

//basically placeholder for now.
