#pragma once

#include <windows.h>
#include "../addons/imgui/imgui.h" // Corrected Include ImGui for ImColor
#include <cmath> // For sqrtf
#include <string> // Required for std::string

// Define 
// and CFrame structs first

struct Matrix3 {
    float data[9]; // Represents a 3x3 matrix

    Matrix3() {
        for (int i = 0; i < 9; ++i) data[i] = 0.0f;
    }
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
        inline bool show_tracers = true;
        inline bool show_box = true;
        inline bool hide_dead = true;
        inline bool hide_teammates = true;
        inline bool show_fps = true;
        inline bool show_weapon = true;

        inline ImColor esp_box_color = ImColor(255, 255, 255, 255);
        inline ImColor esp_name_color = ImColor(255, 255, 255, 255);
        inline ImColor esp_distance_color = ImColor(255, 255, 255, 255);
        inline ImColor esp_skeleton_color = ImColor(255, 0, 0, 255);
        inline ImColor esp_tracer_color = ImColor(255, 255, 255, 255);
    }

    namespace triggerbot
    {
        inline bool toggled = false;
        inline int activation_key = VK_XBUTTON1;
        inline bool ignore_teammates = true;
        inline float fov = 15.0f;
        inline int delay = 50;
        inline int hold_time = 50;
    }

    namespace aimbot
    {
        inline bool toggled = true;
        inline float speed = 1.0f;
        inline float fov = 100.0f;
        inline float deadzone = 5.0f;
        inline bool use_set_cursor_pos = false;
        inline float smoothing_factor = 0.2f;
        inline int activation_key = 0x02;
        inline int aimbot_hitbox = 0; // 0 = Head, 1 = Body ()
        inline bool ignore_teammates = true;
        inline bool show_fov_circle = true; // New: Toggle for FOV circle
        inline bool prediction = false; // New: Toggle for aimbot prediction
    }

    namespace combat {
        inline bool hitbox_expander = false;
        inline float hitbox_size_x = 8.0f;
        inline float hitbox_size_y = 8.0f;
        inline float hitbox_size_z = 8.0f;
        inline float hitbox_multiplier = 2.0f;
        inline bool hitbox_visible = false;
        inline bool hitbox_can_collide = false;
        inline bool hitbox_skip_teammates = true;
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

    namespace set_fov 
    {
        inline float set_fov = 70.0f;
        inline bool toggled = false;
        inline bool unlock_zoom = false;
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

    namespace infinite_jump
    {
        inline bool toggled = false;
        inline float jump_power_value = 5000.0f;
    }

    namespace fly
    {
        inline bool toggled = false;
        inline float speed = 1.0f;
        inline int fly_toggle_key = 0x51; // Q key
        inline int fly_mode = 0; // 0 = Hold, 1 = Toggle
    }

    namespace airswim
    {
        inline bool toggled = false;
    }

    namespace misc
    {
        inline bool show_workspace_viewer = false;
        inline float teleport_offset_y = 5.0f;
        inline float teleport_offset_z = 3.0f;
        inline uintptr_t selected_player_for_info = 0;
        inline std::string spectating_player_name = "";
        inline uintptr_t spectating_camera = 0;
    }

    namespace anti_afk
    {
        inline bool toggled = false;
        inline float interval = 15.0f;
    }

    namespace lag_switch
    {
        inline bool toggled = false;
        inline int activation_key = 0x4C; // L key
        inline float lag_duration = 0.5f; // How long to lag (seconds)
        inline float lag_interval = 2.0f; // How often to lag (seconds)
        inline bool auto_lag = false; // Automatically lag at intervals
        inline bool manual_lag = true; // Manual lag on key press
    }
}