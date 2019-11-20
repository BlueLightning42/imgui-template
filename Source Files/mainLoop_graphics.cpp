#include "pch.h"

#include "graphics.h"

void GraphicContext::main_loop(){
	if (v._SwapChainRebuild) {
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
