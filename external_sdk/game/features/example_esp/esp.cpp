#include "esp.hpp"
#include "../game/core.hpp"
#include "../addons/kernel/memory.hpp"
#include "../game/offsets/offsets.hpp"
#include "../handlers/overlay/draw.hpp"
#include "../addons/imgui/imgui.h"
#include "../handlers/utility/utility.hpp"
#include "../../../handlers/vars.hpp"
#include <chrono>

static auto last_time = std::chrono::high_resolution_clock::now();
static int frame_count = 0;
static float fps = 0.0f;

static std::chrono::steady_clock::time_point trigger_fire_time = {};
static bool trigger_waiting = false;

void c_esp::calculate_fps()
{
    using clock = std::chrono::high_resolution_clock;
    auto now = clock::now();
    frame_count++;

    std::chrono::duration<float> elapsed = now - last_time;
    if (elapsed.count() >= 0.5f) {
        fps = frame_count / elapsed.count();
        frame_count = 0;
        last_time = now;
    }
}

bool c_esp::is_visible(const vector& from, const vector& to, uintptr_t target_model)
{
    return true;
}

void c_esp::run_players(view_matrix_t viewmatrix)
{
    if (vars::esp::show_fps)
    {
        calculate_fps();
        std::stringstream ss_fps;
        ss_fps << "FPS: " << std::fixed << std::setprecision(0) << fps;
        draw.outlined_string(ImVec2(10, 10), ss_fps.str().c_str(), ImColor(0, 255, 0, 255), ImColor(0, 0, 0, 255), false);
    }

    std::vector< uintptr_t > players = core.get_players(g_main::datamodel);

    for (auto& player : players)
    {
        if (!player || player == g_main::localplayer)
            continue;

        if (vars::esp::hide_teammates)
        {
            uintptr_t player_team = memory->read<uintptr_t>(player + offsets::Team);
            if (player_team == g_main::localplayer_team && player_team != 0)
                continue;
        }

        auto model = core.get_model_instance(player);
        if (!model)
            continue;

        auto humanoid = core.find_first_child_class(model, "Humanoid");
        if (!humanoid)
            continue;

        float health = memory->read< float >(humanoid + offsets::Health);
        float max_health = memory->read< float >(humanoid + offsets::MaxHealth);

        if (vars::esp::hide_dead && health <= 0.0f)
            continue;

        if (!health || !max_health)
            continue;

        auto player_root = core.find_first_child(model, "HumanoidRootPart");
        if (!player_root)
            continue;

        auto p_player_root = memory->read< uintptr_t >(player_root + offsets::Primitive);
        if (!p_player_root)
            continue;

        auto player_head = core.find_first_child(model, "Head");
        if (!player_head)
            continue;

        auto p_player_head = memory->read< uintptr_t >(player_head + offsets::Primitive);
        if (!p_player_head)
            continue;

        std::string player_name = core.get_instance_name(player);
        if (player_name.empty())
            continue;

        vector w_player_root = memory->read< vector >(p_player_root + offsets::Position);
        vector w_player_head = memory->read< vector >(p_player_head + offsets::Position);

        float hip_height = memory->read< float >(humanoid + offsets::HipHeight);
        vector w_foot_pos = vector(w_player_root.x, w_player_root.y - hip_height, w_player_root.z);

        vector2d s_foot_pos;
        if (!core.world_to_screen(w_foot_pos, s_foot_pos, viewmatrix))
            continue;

        vector2d s_player_head;
        if (!core.world_to_screen(vector(w_player_head.x, w_player_head.y + 0.5f, w_player_head.z), s_player_head, viewmatrix))
            continue;

        float height = s_foot_pos.y - s_player_head.y;
        float width = height * 0.4f;

        ImVec2 top_left = ImVec2(s_foot_pos.x - (width * 0.5f), s_player_head.y);
        ImVec2 bottom_right = ImVec2(s_foot_pos.x + (width * 0.5f), s_foot_pos.y);

        if (vars::esp::show_box)
        {
            draw.outlined_rectangle(top_left, bottom_right, vars::esp::esp_box_color, ImColor(0, 0, 0, 255), 1.0f);
        }

        if (vars::esp::show_tracers)
        {
            int screen_width = core.get_screen_width();
            int screen_height = core.get_screen_height();
            draw.line(ImVec2(screen_width * 0.5f, screen_height), ImVec2(s_foot_pos.x, s_foot_pos.y), vars::esp::esp_tracer_color, 1.0f);
        }

        float health_percent = max(min(health / max_health, 1.0f), 0.0f);
        ImColor health_color = ImColor(
            static_cast<int>(255 * (1.0f - health_percent)),
            static_cast<int>(255 * health_percent),
            0, 255
        );

        float text_y_offset = top_left.y - 15;
        draw.outlined_string(ImVec2(s_foot_pos.x, text_y_offset), player_name.c_str(), vars::esp::esp_name_color, ImColor(0, 0, 0, 255), true);

        if (vars::esp::show_weapon)
        {
            auto equipped_tool = core.find_first_child_class(model, "Tool");
            if (equipped_tool)
            {
                std::string weapon_name = core.get_instance_name(equipped_tool);
                if (!weapon_name.empty())
                {
                    text_y_offset -= 15;
                    draw.outlined_string(ImVec2(s_foot_pos.x, text_y_offset), weapon_name.c_str(), ImColor(255, 165, 0, 255), ImColor(0, 0, 0, 255), true);
                }
            }
        }

        if (vars::esp::show_health)
        {
            std::stringstream ss_health;
            ss_health << "[HP: " << std::fixed << std::setprecision(0) << health << "]";
            draw.outlined_string(ImVec2(s_foot_pos.x, s_foot_pos.y + 5), ss_health.str().c_str(), health_color, ImColor(0, 0, 0, 255), true);
        }

        if (vars::esp::show_distance)
        {
            uintptr_t local_player_character_model = core.find_first_child(core.find_first_child_class(g_main::datamodel, "Workspace"), core.get_instance_name(g_main::localplayer));
            if (local_player_character_model)
            {
                auto local_player_root = core.find_first_child(local_player_character_model, "HumanoidRootPart");
                if (local_player_root)
                {
                    auto p_local_player_root = memory->read< uintptr_t >(local_player_root + offsets::Primitive);
                    if (p_local_player_root)
                    {
                        vector w_local_player_root = memory->read< vector >(p_local_player_root + offsets::Position);
                        float distance = sqrtf(
                            powf(w_player_root.x - w_local_player_root.x, 2) +
                            powf(w_player_root.y - w_local_player_root.y, 2) +
                            powf(w_player_root.z - w_local_player_root.z, 2)
                        );

                        std::stringstream ss_distance;
                        ss_distance << "[" << std::fixed << std::setprecision(0) << distance << "m]";
                        draw.outlined_string(ImVec2(s_foot_pos.x, s_foot_pos.y + 20), ss_distance.str().c_str(), vars::esp::esp_distance_color, ImColor(0, 0, 0, 255), true);
                    }
                }
            }
        }

        if (vars::esp::show_skeleton)
        {
            struct BoneInfo { const char* name; const char* parent_name; };
            BoneInfo skeleton_bones[] = {
                {"Head", "Torso"},
                {"Torso", "HumanoidRootPart"},
                {"Right Arm", "Torso"},
                {"Left Arm", "Torso"},
                {"Right Leg", "Torso"},
                {"Left Leg", "Torso"}
            };

            std::unordered_map<std::string, vector2d> bone_screen_positions;

            for (const auto& bone_info : skeleton_bones)
            {
                uintptr_t bone_part = core.find_first_child(model, bone_info.name);
                if (bone_part)
                {
                    uintptr_t p_bone_part = memory->read<uintptr_t>(bone_part + offsets::Primitive);
                    if (p_bone_part)
                    {
                        vector w_bone_pos = memory->read<vector>(p_bone_part + offsets::Position);
                        vector2d s_bone_pos;
                        if (core.world_to_screen(w_bone_pos, s_bone_pos, viewmatrix))
                        {
                            bone_screen_positions[bone_info.name] = s_bone_pos;
                        }
                    }
                }
            }

            for (const auto& bone_info : skeleton_bones)
            {
                if (bone_screen_positions.count(bone_info.name) && bone_screen_positions.count(bone_info.parent_name))
                {
                    draw.line(
                        ImVec2(bone_screen_positions[bone_info.name].x, bone_screen_positions[bone_info.name].y),
                        ImVec2(bone_screen_positions[bone_info.parent_name].x, bone_screen_positions[bone_info.parent_name].y),
                        vars::esp::esp_skeleton_color, 1.0f
                    );
                }
            }
        }
    }
}

void c_esp::run_aimbot(view_matrix_t viewmatrix)
{
    vector2d crosshair_pos = { static_cast<float>(core.get_screen_width() / 2), static_cast<float>(core.get_screen_height() / 2) };

    if (vars::aimbot::show_fov_circle)
    {
        draw.circle(ImVec2(crosshair_pos.x, crosshair_pos.y), vars::aimbot::fov, ImColor(255, 255, 255, 255), 1.0f);
    }

    bool aimbot_active = vars::aimbot::toggled && GetAsyncKeyState(vars::aimbot::activation_key);
    bool triggerbot_active = vars::triggerbot::toggled && GetAsyncKeyState(vars::triggerbot::activation_key);

    if (!aimbot_active && !triggerbot_active)
    {
        this->leftover_x = 0.0f;
        this->leftover_y = 0.0f;
        this->smoothed_delta_x = 0.0f;
        this->smoothed_delta_y = 0.0f;
        this->locked_target = 0;
        trigger_waiting = false;
        return;
    }

    uintptr_t workspace = core.find_first_child_class(g_main::datamodel, "Workspace");
    uintptr_t camera = core.find_first_child_class(workspace, "Camera");
    vector cam_pos = {};
    if (camera)
        cam_pos = memory->read<vector>(camera + offsets::CameraPos);

    std::vector< uintptr_t > players = core.get_players(g_main::datamodel);
    uintptr_t current_target = 0;

    if (this->locked_target != 0 && aimbot_active)
    {
        bool locked_target_still_valid = false;
        for (auto& player : players)
        {
            if (player == this->locked_target)
            {
                uintptr_t player_team = memory->read<uintptr_t>(player + offsets::Team);
                if (player_team == g_main::localplayer_team && player_team != 0)
                {
                    this->locked_target = 0;
                    break;
                }

                auto model = core.get_model_instance(player);
                if (model)
                {
                    auto humanoid = core.find_first_child_class(model, "Humanoid");
                    if (humanoid)
                    {
                        float health = memory->read< float >(humanoid + offsets::Health);
                        if (health > 0.0f)
                        {
                            const char* target_bone_name = (vars::aimbot::aimbot_hitbox == 0) ? "Head" : "HumanoidRootPart";
                            auto target_bone = core.find_first_child(model, target_bone_name);
                            if (target_bone)
                            {
                                auto p_target_bone = memory->read< uintptr_t >(target_bone + offsets::Primitive);
                                if (p_target_bone)
                                {
                                    vector w_target_bone_pos = memory->read< vector >(p_target_bone + offsets::Position);

                                    if (camera && !is_visible(cam_pos, w_target_bone_pos, model))
                                    {
                                        this->locked_target = 0;
                                        break;
                                    }

                                    vector2d s_target_bone_pos;
                                    if (core.world_to_screen(w_target_bone_pos, s_target_bone_pos, viewmatrix))
                                    {
                                        float distance = sqrt(pow(s_target_bone_pos.x - crosshair_pos.x, 2) + pow(s_target_bone_pos.y - crosshair_pos.y, 2));
                                        if (distance < vars::aimbot::fov)
                                        {
                                            locked_target_still_valid = true;
                                            current_target = this->locked_target;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (!locked_target_still_valid)
        {
            this->locked_target = 0;
        }
    }

    if (this->locked_target == 0)
    {
        uintptr_t new_closest_player = 0;
        float new_closest_distance = FLT_MAX;
        float search_fov = aimbot_active ? vars::aimbot::fov : vars::triggerbot::fov;

        for (auto& player : players)
        {
            if (!player || player == g_main::localplayer)
                continue;

            uintptr_t player_team = memory->read<uintptr_t>(player + offsets::Team);
            if (player_team == g_main::localplayer_team && player_team != 0)
                continue;

            auto model = core.get_model_instance(player);
            if (!model)
                continue;

            auto humanoid = core.find_first_child_class(model, "Humanoid");
            if (!humanoid)
                continue;

            float health = memory->read< float >(humanoid + offsets::Health);
            if (health <= 0.0f)
                continue;

            auto player_root = core.find_first_child(model, "HumanoidRootPart");
            if (!player_root)
                continue;

            auto p_player_root = memory->read< uintptr_t >(player_root + offsets::Primitive);
            if (!p_player_root)
                continue;

            const char* target_bone_name = (vars::aimbot::aimbot_hitbox == 0) ? "Head" : "HumanoidRootPart";
            auto target_bone = core.find_first_child(model, target_bone_name);
            if (!target_bone)
                continue;
            auto p_target_bone = memory->read< uintptr_t >(target_bone + offsets::Primitive);
            if (!p_target_bone)
                continue;

            vector w_target_bone_pos = memory->read< vector >(p_target_bone + offsets::Position);
            vector v_player_root = memory->read< vector >(p_player_root + offsets::Velocity);

            if (vars::aimbot::prediction) {
                w_target_bone_pos = w_target_bone_pos + (v_player_root * 0.1f);
            }

            if (camera && !is_visible(cam_pos, w_target_bone_pos, model))
                continue;

            vector2d s_target_bone_pos;
            if (!core.world_to_screen(w_target_bone_pos, s_target_bone_pos, viewmatrix))
                continue;

            float distance = sqrt(pow(s_target_bone_pos.x - crosshair_pos.x, 2) + pow(s_target_bone_pos.y - crosshair_pos.y, 2));

            if (distance < new_closest_distance && distance < search_fov)
            {
                new_closest_distance = distance;
                new_closest_player = player;
            }
        }
        current_target = new_closest_player;
        if (aimbot_active)
            this->locked_target = new_closest_player;
    }

    if (current_target)
    {
        auto model = core.get_model_instance(current_target);
        auto player_root = core.find_first_child(model, "HumanoidRootPart");
        auto p_player_root = memory->read< uintptr_t >(player_root + offsets::Primitive);
        vector v_player_root = memory->read< vector >(p_player_root + offsets::Velocity);

        const char* target_bone_name = (vars::aimbot::aimbot_hitbox == 0) ? "Head" : "HumanoidRootPart";
        auto target_bone = core.find_first_child(model, target_bone_name);
        auto p_target_bone = memory->read< uintptr_t >(target_bone + offsets::Primitive);
        vector w_target_bone_pos = memory->read< vector >(p_target_bone + offsets::Position);

        vector2d s_target_bone_pos;
        if (core.world_to_screen(w_target_bone_pos, s_target_bone_pos, viewmatrix))
        {
            float delta_x = s_target_bone_pos.x - crosshair_pos.x;
            float delta_y = s_target_bone_pos.y - crosshair_pos.y;

            if (triggerbot_active)
            {
                float distance = sqrt(pow(delta_x, 2) + pow(delta_y, 2));

                if (distance < vars::triggerbot::fov)
                {
                    auto now = std::chrono::steady_clock::now();

                    if (!trigger_waiting) {
                        trigger_fire_time = now + std::chrono::milliseconds(vars::triggerbot::delay);
                        trigger_waiting = true;
                    }

                    if (now >= trigger_fire_time) {
                        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);

                        auto release_time = now + std::chrono::milliseconds(vars::triggerbot::hold_time);
                        while (std::chrono::steady_clock::now() < release_time) {
                        }

                        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                        trigger_waiting = false;
                    }
                }
                else {
                    trigger_waiting = false;
                }
            }

            if (aimbot_active)
            {
                this->smoothed_delta_x = (delta_x * vars::aimbot::smoothing_factor) + (this->smoothed_delta_x * (1.0f - vars::aimbot::smoothing_factor));
                this->smoothed_delta_y = (delta_y * vars::aimbot::smoothing_factor) + (this->smoothed_delta_y * (1.0f - vars::aimbot::smoothing_factor));

                if (abs(this->smoothed_delta_x) < vars::aimbot::deadzone && abs(this->smoothed_delta_y) < vars::aimbot::deadzone)
                {
                    this->leftover_x = 0.0f;
                    this->leftover_y = 0.0f;
                    return;
                }

                float aim_delta_x = this->smoothed_delta_x / vars::aimbot::speed;
                float aim_delta_y = this->smoothed_delta_y / vars::aimbot::speed;

                if (vars::aimbot::use_set_cursor_pos)
                {
                    int target_x = static_cast<int>(s_target_bone_pos.x);
                    int target_y = static_cast<int>(s_target_bone_pos.y);

                    POINT current_mouse_pos;
                    GetCursorPos(&current_mouse_pos);

                    float smooth_factor = 1.0f / vars::aimbot::speed;
                    int move_x = static_cast<int>(current_mouse_pos.x + (target_x - current_mouse_pos.x) * smooth_factor);
                    int move_y = static_cast<int>(current_mouse_pos.y + (target_y - current_mouse_pos.y) * smooth_factor);

                    if (move_x == current_mouse_pos.x && target_x != current_mouse_pos.x) {
                        move_x += (target_x > current_mouse_pos.x ? 1 : -1);
                    }
                    if (move_y == current_mouse_pos.y && target_y != current_mouse_pos.y) {
                        move_y += (target_y > current_mouse_pos.y ? 1 : -1);
                    }

                    SetCursorPos(move_x, move_y);
                    this->leftover_x = 0.0f;
                    this->leftover_y = 0.0f;
                }
                else
                {
                    aim_delta_x += this->leftover_x;
                    aim_delta_y += this->leftover_y;

                    LONG move_x = static_cast<LONG>(aim_delta_x);
                    LONG move_y = static_cast<LONG>(aim_delta_y);

                    this->leftover_x = aim_delta_x - move_x;
                    this->leftover_y = aim_delta_y - move_y;

                    INPUT input = { 0 };
                    input.type = INPUT_MOUSE;
                    input.mi.dx = move_x;
                    input.mi.dy = move_y;
                    input.mi.dwFlags = MOUSEEVENTF_MOVE;
                    SendInput(1, &input, sizeof(INPUT));
                }
            }
        }
    }
    else
    {
        this->leftover_x = 0.0f;
        this->leftover_y = 0.0f;
        this->smoothed_delta_x = 0.0f;
        this->smoothed_delta_y = 0.0f;
        trigger_waiting = false;
    }
}