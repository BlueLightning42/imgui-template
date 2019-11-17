#include "pch.h"

#include "graphics.h"
#include "load_save.h"

int main(){
	//load saved user settings
	user_info settings = load_user_info();
	//setup SDL window
	Gui gui(settings);
	//setup vulkan bare bones. Instance/Device/Surface.
	Vulkan v(gui.sdl_window);

	gui.bind_vulkan(&v);
	
	gui.initialize();


	// Main loop
	bool done = false;
	while (!done)
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED && event.window.windowID == SDL_GetWindowID(gui.sdl_window))
			{
				v._SwapChainResizeWidth = (int)event.window.data1;
				v._SwapChainResizeHeight = (int)event.window.data2;
				v._SwapChainRebuild = true;
			}
		}

		if (v._SwapChainRebuild)
		{
			v._SwapChainRebuild = false;
			ImGui_ImplVulkan_SetMinImageCount(v._MinImageCount);
			ImGui_ImplVulkanH_CreateWindow(v.c_Instance(), v.c_PhysicalDevice(), v.c_Device(), &gui.wd, v._QueueFamilyIndex, v.c_ptr_Allocator(), v._SwapChainResizeWidth, v._SwapChainResizeHeight, v._MinImageCount);
			gui.setFrameIndex(0);
		}

		// Start the Dear ImGui frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame(gui.sdl_window);
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (gui.show_demo_window)
			ImGui::ShowDemoWindow(&gui.show_demo_window);


		// Rendering
		gui.clear();

		gui.FrameRender();

		gui.FramePresent();
	}

	// Cleanup
	gui.cleanup_imgui();

	v.cleanup_vulkan();
	gui.cleanup_sdl();


    return 0;
}
