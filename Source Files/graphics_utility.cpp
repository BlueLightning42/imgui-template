#include "pch.h"

#include "graphics.h"

// could make these nice and templated for now leaving them as overloaded functions...also helps with it not being in a header file
// basically a use for vulkan objects in both c and c++ bindings along with a general bool type.
// version that just displays text and a version that lets you format a parameter.

// Custom Assert + text if assert fails
void check_result(const vk::Result result, const char* description) noexcept {
	if (result != vk::Result::eSuccess) {
		fmt::print(stderr, "\n{}\nError: {}\n", description, to_string(result));
		std::exit(EXIT_FAILURE);
	}
}

void check_result(const VkResult result, const char* description) noexcept {
	if (result != VK_SUCCESS) {
		fmt::print(stderr, "\n{}\nError: {}\n", description, to_string(vk::Result(result)));
		std::exit(EXIT_FAILURE);
	}
}

// NOTE bound to sdl/imgui
void check_result(const VkResult result) noexcept {
	if (result != VK_SUCCESS) {
		fmt::print(stderr, "\nError: {}\n", to_string(vk::Result(result)));
		if (result < 0)
			std::exit(EXIT_FAILURE);
	}
}

void check_result(const bool result, const char* description) noexcept {
	if (result != true) {
		fmt::print(stderr, "\nError: {}\n", description);
		std::exit(EXIT_FAILURE);
	}
}
// these two have format args
void check_result(const bool result, const char* format, fmt::format_args args) noexcept {
	if (result != true) {
		fmt::vprint(stderr, format, args);	
		std::exit(EXIT_FAILURE);
	}
}

void check_result(const vk::Result result, const char* description, fmt::format_args args) noexcept {
	if (result != vk::Result::eSuccess) {
		fmt::vprint(description, args);
		fmt::print(stderr, "\nError: {}\n", to_string(result));
		std::exit(EXIT_FAILURE);
	}
}




void Gui::clear() {
	ImGui::Render();
	memcpy(&wd.ClearValue.color.float32[0], &clear_color, 4 * sizeof(float));
}

void Gui::FrameRender() {
	VkResult err;

	VkSemaphore image_acquired_semaphore = wd.FrameSemaphores[wd.SemaphoreIndex].ImageAcquiredSemaphore;
	VkSemaphore render_complete_semaphore = wd.FrameSemaphores[wd.SemaphoreIndex].RenderCompleteSemaphore;
	err = vkAcquireNextImageKHR(v->c_Device(), wd.Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd.FrameIndex);
	check_result(err);

	ImGui_ImplVulkanH_Frame* fd = &wd.Frames[wd.FrameIndex];
	{
		err = vkWaitForFences(v->c_Device(), 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
		check_result(err);

		err = vkResetFences(v->c_Device(), 1, &fd->Fence);
		check_result(err);
	}
	{
		err = vkResetCommandPool(v->c_Device(), fd->CommandPool, 0);
		check_result(err);
		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
		check_result(err);
	}
	{
		VkRenderPassBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		info.renderPass = wd.RenderPass;
		info.framebuffer = fd->Framebuffer;
		info.renderArea.extent.width = wd.Width;
		info.renderArea.extent.height = wd.Height;
		info.clearValueCount = 1;
		info.pClearValues = &wd.ClearValue;
		vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
	}

	// Record Imgui Draw Data and draw funcs into command buffer
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), fd->CommandBuffer);

	// Submit command buffer
	vkCmdEndRenderPass(fd->CommandBuffer);
	{
		VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &image_acquired_semaphore;
		info.pWaitDstStageMask = &wait_stage;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &fd->CommandBuffer;
		info.signalSemaphoreCount = 1;
		info.pSignalSemaphores = &render_complete_semaphore;

		err = vkEndCommandBuffer(fd->CommandBuffer);
		check_result(err);
		err = vkQueueSubmit(v->c_Queue(), 1, &info, fd->Fence);
		check_result(err);
	}
}

void Gui::FramePresent() {
	VkSemaphore render_complete_semaphore = wd.FrameSemaphores[wd.SemaphoreIndex].RenderCompleteSemaphore;
	VkPresentInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores = &render_complete_semaphore;
	info.swapchainCount = 1;
	info.pSwapchains = &wd.Swapchain;
	info.pImageIndices = &wd.FrameIndex;
	VkResult err = vkQueuePresentKHR(v->c_Queue(), &info);
	check_result(err);
	wd.SemaphoreIndex = (wd.SemaphoreIndex + 1) % wd.ImageCount; // Now we can use the next set of semaphores
}

