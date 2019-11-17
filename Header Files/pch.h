// because visual studio... and decreasing build times

#pragma once

// Enable the WSI extensions
#if defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#elif defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED
#define VULKAN_HPP_NO_EXCEPTIONS


// Important STL
#include <vector>
#include <cstdlib>

//not working...fucking annoying
#pragma warning(push, 0)

// Graphics
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>à
#include <vulkan/vulkan.hpp>
#include <imgui.h>
#include "imgui_impl_sdl.h"
#include "imgui_impl_vulkan.h"

//nice to have librarys
#include <fmt/format.h>
#include <fmt/ostream.h>
#pragma warning(pop) 

//using's
using vec_cstr = std::vector<const char*>;

// stable project headers
