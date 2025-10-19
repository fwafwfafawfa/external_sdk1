#pragma once

#include <windows.h>
#include "../addons/imgui/imgui.h" // Corrected Include ImGui for ImColor
#include <cmath> // For sqrtf

// Define vector and CFrame structs first
struct vector {
    float x, y, z;

    vector ( ) : x ( 0 ), y ( 0 ), z ( 0 ) {}
    vector ( float x, float y, float z ) : x ( x ), y ( y ), z ( z ) {}

    vector operator+(const vector& other) const { return vector(x + other.x, y + other.y, z + other.z); }
    vector operator-(const vector& other) const { return vector(x - other.x, y - other.y, z - other.z); }
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
};

struct CFrame {
    vector Position;
    float R00, R01, R02;
    float R10, R11, R12;
    float R20, R21, R22;

    CFrame() : Position({0,0,0}),
               R00(1), R01(0), R02(0),
               R10(0), R11(1), R12(0),
               R20(0), R21(0), R22(1) {}
};

// Now define the vars namespace that uses the vector struct
namespace vars
{
    namespace esp
    {
        inline bool toggled = true;
        inline bool show_health = true;
        inline bool show_distance = true;
        inline bool show_skeleton = false;

        inline ImColor esp_box_color = ImColor(255, 255, 255, 255);
        inline ImColor esp_name_color = ImColor(255, 255, 255, 255);
        inline ImColor esp_distance_color = ImColor(255, 255, 255, 255);
        inline ImColor esp_skeleton_color = ImColor(255, 0, 0, 255);
    }

    namespace aimbot
    {
        inline bool toggled = true;
        inline float speed = 10.0f;
        inline float fov = 100.0f;
        inline float deadzone = 5.0f;
        inline bool use_set_cursor_pos = false;
        inline float smoothing_factor = 0.2f;
        inline int activation_key = 0x02;
        inline int aimbot_hitbox = 0; // 0 = Head, 1 = Body (HumanoidRootPart)
        inline bool show_fov_circle = true; // New: Toggle for FOV circle
        inline bool prediction = false; // New: Toggle for aimbot prediction
    }

    namespace speed_hack
    {
        inline bool toggled = false;
        inline float value = 16.0f;
    }

    namespace freecam
    {
        inline bool toggled = false;
        inline float speed = 2.0f;
        inline float sensitivity = 0.002f;
    }

    namespace noclip
    {
        inline bool toggled = false;
    }

    namespace jump_power
    {
        inline bool toggled = false;
        inline float value = 50.0f;
        inline float default_value = 50.0f;
    }

    namespace fly
    {
        inline bool toggled = false;
        inline float speed = 1.0f;
    }

    namespace misc
    {
        inline bool show_workspace_viewer = false;
        inline float teleport_offset_y = 5.0f;
        inline float teleport_offset_z = 3.0f;
        inline uintptr_t selected_player_for_info = 0;
    }

    namespace anti_afk
    {
        inline bool toggled = false;
        inline float interval = 15.0f;
    }
}