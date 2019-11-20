#pragma once

#include "pch.h"

#include "imgui_impl_sdl.h"
#include "imgui_impl_vulkan.h"

#include "load_save.h"

void check_result(const vk::Result,	const char*) noexcept;
void check_result(const VkResult,	const char*) noexcept;
// NOTE bound to sdl/imgui
void check_result(const VkResult)	noexcept;
void check_result(const bool,		const char*) noexcept;
// format args
void check_result(const bool,		const char*, fmt::format_args) noexcept;
void check_result(const vk::Result,	const char*, fmt::format_args) noexcept;






class Vulkan {
public:
	vk::AllocationCallbacks* _Allocator			= NULL;
	vk::Instance             _Instance			= VK_NULL_HANDLE;
	vk::PhysicalDevice       _PhysicalDevice	= VK_NULL_HANDLE;
	vk::Device               _Device			= VK_NULL_HANDLE;
	uint32_t                 _QueueFamilyIndex	= (uint32_t)-1;
	vk::Queue                _Queue				= VK_NULL_HANDLE;
	VkDebugReportCallbackEXT _DebugReport		= VK_NULL_HANDLE;
	vk::PipelineCache        _PipelineCache		= VK_NULL_HANDLE;
	vk::DescriptorPool       _DescriptorPool	= VK_NULL_HANDLE;

	vk::SurfaceKHR			_surface;

	std::vector<const char*> _extensions;
	uint32_t _extensions_count;

	SDL_Window* _window;


	uint32_t  _MinImageCount = 2;
	bool      _SwapChainRebuild = false;
	int       _SwapChainResizeWidth = 0;
	int       _SwapChainResizeHeight = 0;

	//interface with c library functions... just wrappers for static casts
	VkInstance c_Instance()	    	     { return static_cast<VkInstance>(_Instance);  }
	VkPhysicalDevice c_PhysicalDevice()  { return static_cast<VkPhysicalDevice>(_PhysicalDevice);  }
	VkDevice c_Device()    	             { return static_cast<VkDevice>(_Device);  }
	VkQueue c_Queue()    	             { return static_cast<VkQueue>(_Queue);	}
	VkPipelineCache c_PipelineCache()	 { return static_cast<VkPipelineCache>(_PipelineCache); }
	VkDescriptorPool c_DescriptorPool()  { return static_cast<VkDescriptorPool>(_DescriptorPool); }

	//fuck these ones... leaving ptr in the name to be more explicit.
	VkSurfaceKHR* c_ptr_SurfaceKHR()          { return reinterpret_cast<VkSurfaceKHR*>(&_surface); } 
	VkAllocationCallbacks* c_ptr_Allocator() { return reinterpret_cast<VkAllocationCallbacks*>(_Allocator); }

	// TODO make other constructors for different window types than SDL.
	// basically low priority todo decouple from sdl
	Vulkan(SDL_Window *window):
		_window(window) {
		this->setup_vulkan();
		this->create_surface();
	}
	Vulkan() = default;


	void cleanup_vulkan();

 private:
	void create_instance();
	int rate_physical_device(const vk::PhysicalDevice&);
	void pick_physical_device();
	uint32_t get_queue_index(vk::QueueFlagBits);
	void select_graphics_queue_family();
	void create_logical_device();
	void create_descriptor_pool();

	void setup_vulkan();

	void create_surface();
};



class Gui {
 public:
	SDL_Window* sdl_window;

	ImGui_ImplVulkanH_Window wd;
	Vulkan* v;
	UserInfo* settings;
	bool show_demo_window;
	bool show_another_window;
	ImVec4 clear_color;
	Gui(UserInfo);
	
	Gui() = default;

	void bind_vulkan(Vulkan* _v) { this->v = _v; }

	void initialize() {
		setup_vulkan_window();
		setup_Imgui();
	}
	void clear();
	void setFrameIndex(int _f) { wd.FrameIndex = _f; }
	void FrameRender();
	void FramePresent();

	void cleanup_imgui();
	void cleanup_sdl();
 private:
	void setup_vulkan_window();
	void setup_Imgui();
};


class GraphicContext {
public:
	Vulkan v;
	Gui gui;
	UserInfo settings;

	GraphicContext() {
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
	~GraphicContext() {
		// Cleanup
		SDL_GetWindowSize(gui.sdl_window, (int*)&settings.win_width, (int*)&settings.win_height);

		gui.cleanup_imgui();

		v.cleanup_vulkan();
		gui.cleanup_sdl();

		store_user_info();
	}
	void process_event(SDL_Event);
	void main_loop();
private:

	void load_user_info();
	void store_user_info();
};
