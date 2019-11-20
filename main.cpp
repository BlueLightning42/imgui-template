#include "pch.h"

#include "graphics.h"
#include "load_save.h"
#include "game_manager.h"

// instead of a "App" Class everythings brought into main.
// seperate classes seperate the logic for different contexts.

int main(){
	// setup window context
	GraphicContext graphics;
	// dont setup game manager unstil setup() is called.
	GameContext gmanager;

	// Main  Mainloop
	bool done = false;
	while (!done) {
		SDL_Event event;
		while (SDL_PollEvent(&event)){
			ImGui_ImplSDL2_ProcessEvent(&event);
			gmanager.process_event(event);
			graphics.process_event(event);

			if (event.type == SDL_QUIT) done = true;
		}
		gmanager.main_loop(); // update game data
		graphics.main_loop(); // update screen
	}

    return 0;
}
