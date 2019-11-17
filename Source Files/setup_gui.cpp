#include "pch.h"

#include "graphics.h"
#include "load_save.h"

Gui::Gui(user_info settings) {

	check_result(!SDL_Init(SDL_INIT_VIDEO),
		"Could not initialize SDL. ERR: {}", fmt::make_format_args(SDL_GetError()));

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

	sdl_window = SDL_CreateWindow("Temp Name", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, settings.win_width, settings.win_height, window_flags);

	check_result((sdl_window != NULL), "Could not create SDL window.");

}

//Gui Related
void Gui::setup_vulkan_window() {
	wd.Surface = *v->c_ptr_SurfaceKHR();


	// Select Surface Format
	const VkFormat requestSurfaceImageFormat[ ] = {VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
	const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	wd.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(v->c_PhysicalDevice(), wd.Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

	// Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
	VkPresentModeKHR present_modes[ ] = {VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR};
#else
	VkPresentModeKHR present_modes[ ] = {VK_PRESENT_MODE_FIFO_KHR};
#endif
	wd.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(v->c_PhysicalDevice(), wd.Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
	//printf("[vulkan] Selected PresentMode = %d\n", wd.PresentMode);

	VkAllocationCallbacks* allocator = v->c_ptr_Allocator();

	// Create SwapChain, RenderPass, Framebuffer, etc.
	IM_ASSERT(v->_MinImageCount >= 2);
	ImGui_ImplVulkanH_CreateWindow(v->c_Instance(),
		v->c_PhysicalDevice(),
		v->c_Device(),
		&wd,
		v->_QueueFamilyIndex,
		allocator,
		v->width,
		v->height,
		v->_MinImageCount);
}


void Gui::setup_Imgui() {

	VkResult err;
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForVulkan(v->_window);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = v->c_Instance();
	init_info.PhysicalDevice = v->c_PhysicalDevice();
	init_info.Device = v->c_Device();
	init_info.QueueFamily = v->_QueueFamilyIndex;
	init_info.Queue = v->c_Queue();
	init_info.PipelineCache = v->c_PipelineCache();
	init_info.DescriptorPool = v->c_DescriptorPool();
	init_info.Allocator = v->c_ptr_Allocator();
	init_info.MinImageCount = v->_MinImageCount;
	init_info.ImageCount = wd.ImageCount;
	init_info.CheckVkResultFn = check_result;
	ImGui_ImplVulkan_Init(&init_info, wd.RenderPass);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	// Upload Fonts
	{
		// Use any command queue
		VkCommandPool command_pool = wd.Frames[wd.FrameIndex].CommandPool;
		VkCommandBuffer command_buffer = wd.Frames[wd.FrameIndex].CommandBuffer;

		err = vkResetCommandPool(v->c_Device(), command_pool, 0);
		check_result(err);
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(command_buffer, &begin_info);
		check_result(err);

		ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

		VkSubmitInfo end_info = {};
		end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		end_info.commandBufferCount = 1;
		end_info.pCommandBuffers = &command_buffer;
		err = vkEndCommandBuffer(command_buffer);
		check_result(err);
		err = vkQueueSubmit(v->c_Queue(), 1, &end_info, VK_NULL_HANDLE);
		check_result(err);

		err = vkDeviceWaitIdle(v->c_Device());
		check_result(err);
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	// Our state
	this->show_demo_window = true;
	this->show_another_window = false;
	this->clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}
