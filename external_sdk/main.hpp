#pragma once

// -- Required for basic code functionality --
#include "windows.h"
#include <stdio.h>
#include <dwmapi.h>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <random>

// -- DirectX Related --
#include <d3d11.h>
#include <dxgi.h>

// -- Iostream
#include <iostream>

// -- Files --
#include "handlers/utility/utility.hpp"
#include "handlers/overlay/overlay.hpp"
#include "handlers/overlay/draw.hpp"
#include "handlers/menu/menu.hpp"
#include "handlers/vars.hpp"
#include "game/offsets/offsets.hpp"
#include "game/core.hpp"
#include "game/rescan/rescan.hpp"
#include "handlers/config/config.hpp" // Include config.hpp
#include "handlers/misc/misc.hpp" // Include misc.hpp

// -- Kernel --
#include "addons/kernel/driver.hpp"

// -- ImGui Rendering --
#include "addons/imgui/imgui.h"
#include "addons/imgui/imgui_impl_dx11.h"
#include "addons/imgui/imgui_impl_win32.h"

inline namespace g_main
{
	inline uintptr_t datamodel;
	inline uintptr_t v_engine;
	inline uintptr_t localplayer;
	inline uintptr_t localplayer_team;
};

void reinitialize_game_pointers();