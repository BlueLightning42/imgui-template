#include "pch.h"
#include "graphics.h"

GraphicContext::GraphicContext() {
	//load saved user settings
	load_user_info();
	//setup SDL window
	gui = Gui(settings);
	//setup vulkan bare bones. Instance/Device/Surface.
	v = Vulkan(gui.sdl_window);
	//setup imgui
	gui.bind_vulkan(&v);

	gui.initialize();
}

// Note Graphics Setup seperated into vulkan setup and sdl/imgui setup. "setup_gui" and "setup_vulkan" files
