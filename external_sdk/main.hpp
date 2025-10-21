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
#include <cmath> // For sqrtf in vector struct

// -- DirectX Related --
#include <d3d11.h>
#include <dxgi.h>

// -- Iostream
#include <iostream>

// -- Custom Structs --
struct vector {
    float x, y, z;

    vector() : x(0), y(0), z(0) {}
    vector(float x, float y, float z) : x(x), y(y), z(z) {}

    vector operator+(const vector& other) const { return vector(x + other.x, y + other.y, z + other.z); }
    vector operator-(const vector& other) const { return vector(x - other.x, y - other.y, z - other.y); }
    vector operator*(float scalar) const { return vector(x * scalar, y * scalar, z * scalar); }

    float magnitude() const {
        return sqrtf(x * x + y * y + z * z);
    }

    vector unit() const {
        float mag = magnitude();
        if (mag > 0) {
            return vector(x / mag, y / mag, z / mag);
        }
        return vector(0, 0, 0);
    }

    bool IsZero() const {
        return x == 0.0f && y == 0.0f && z == 0.0f;
    }

    void Normalize() {
        float mag = magnitude();
        if (mag > 0) {
            x /= mag;
            y /= mag;
            z /= mag;
        }
    }
};

struct CFrame {
    float R00, R01, R02; // Rotation matrix row 0
    float R10, R11, R12; // Rotation matrix row 1
    float R20, R21, R22; // Rotation matrix row 2
    float X, Y, Z;       // Position vector
};

struct view_matrix_t {
    float m[4][4];

    view_matrix_t() {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m[i][j] = 0.0f;
    }

    view_matrix_t(float values[4][4]) {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m[i][j] = values[i][j];
    }
};

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
#include "addons/kernel/memory.hpp"

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