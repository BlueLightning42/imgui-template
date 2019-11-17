#include "pch.h"

#include "graphics.h"

void Vulkan::cleanup_vulkan() {
	this->_Queue.waitIdle();
	this->_Device.waitIdle();

	this->_Device.destroyDescriptorPool(this->_DescriptorPool, this->_Allocator);

	this->_Device.destroy(this->_Allocator);
#ifdef DEBUG
	debug::freeDebugCallback(instance);
#endif // DEBUG
	this->_Instance.destroy(this->_Allocator);
}

void Gui::cleanup_imgui() {
	v->_Device.waitIdle();

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	ImGui_ImplVulkanH_DestroyWindow(v->c_Instance(), v->c_Device(), &wd, v->c_ptr_Allocator());
}

void Gui::cleanup_sdl() {
	SDL_DestroyWindow(v->_window);
	SDL_Quit();
}
