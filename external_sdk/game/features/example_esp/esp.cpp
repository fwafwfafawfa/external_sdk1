#include "esp.hpp"
#include "../game/core.hpp"
#include "../addons/kernel/memory.hpp"
#include "../game/offsets/offsets.hpp"
#include "../handlers/overlay/draw.hpp"
#include "../addons/imgui/imgui.h"
#include "../handlers/utility/utility.hpp"
#include "../../../handlers/vars.hpp"
#include <chrono>
#include <mutex>
#include <thread>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>

std::vector<WorldPart> c_esp::geometry;
double c_esp::last_refresh = 0.0;
std::atomic<bool> c_esp::building{ false };
std::atomic<bool> c_esp::ready{ false };
std::unordered_map<uintptr_t, std::pair<bool, std::chrono::steady_clock::time_point>> c_esp::vis_cache;
std::mutex c_esp::geometry_mtx;
std::mutex c_esp::vis_cache_mtx;
std::unordered_map<uintptr_t, TargetData> c_esp::target_tracking;
std::mutex c_esp::tracking_mtx;
std::mutex c_esp::players_mtx;


std::atomic<bool> c_esp::hitbox_thread_running{ false };
int c_esp::hitbox_processed_count = 0;


static inline vector normalize_vec(const vector& v)
{
    float mag = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (mag < 1e-6f) return { 0.f, 0.f, 0.f };
    return { v.x / mag, v.y / mag, v.z / mag };
}

static inline float magnitude_vec(const vector& v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

static inline bool check_ray_box(const vector& origin, const vector& dir, const WorldPart& part, float dist)
{
    vector to_part{ part.pos.x - origin.x, part.pos.y - origin.y, part.pos.z - origin.z };
    float radius = sqrtf(part.size.x * part.size.x + part.size.y * part.size.y + part.size.z * part.size.z) * 0.866f;
    float dist_sq = to_part.x * to_part.x + to_part.y * to_part.y + to_part.z * to_part.z;

    if (dist_sq > (radius + dist) * (radius + dist))
        return false;

    vector local_o{ origin.x - part.pos.x, origin.y - part.pos.y, origin.z - part.pos.z };
    vector half{ part.size.x * 0.5f, part.size.y * 0.5f, part.size.z * 0.5f };

    float tmin = -FLT_MAX, tmax = FLT_MAX;

    for (int i = 0; i < 3; i++)
    {
        float o = (i == 0) ? local_o.x : (i == 1) ? local_o.y : local_o.z;
        float d = (i == 0) ? dir.x : (i == 1) ? dir.y : dir.z;
        float h = (i == 0) ? half.x : (i == 1) ? half.y : half.z;

        if (std::fabs(d) < 1e-6f)
        {
            if (o < -h || o > h)
                return false;
        }
        else
        {
            float t1 = (-h - o) / d;
            float t2 = (h - o) / d;
            if (t1 > t2) std::swap(t1, t2);
            if (t1 > tmin) tmin = t1;
            if (t2 < tmax) tmax = t2;
            if (tmin > tmax || tmax < 0.0f || tmin > dist)
                return false;
        }
    }

    return (tmin > 0.0f || tmax > 0.0f) && tmin <= dist && tmin <= tmax;
}

static auto last_time = std::chrono::high_resolution_clock::now();
static int frame_count = 0;
static float fps = 0.0f;
static std::chrono::steady_clock::time_point trigger_fire_time = {};
static bool trigger_waiting = false;

void c_esp::calculate_fps()
{
    // Remove this entire function or leave it empty
}

void c_esp::update_world_cache()
{
    if (building) return;
    building = true;

    std::thread([]()
        {
            std::vector<WorldPart> parts;
            parts.reserve(3000);

            uintptr_t workspace = core.find_first_child_class(g_main::datamodel, "Workspace");
            if (!workspace)
            {
                std::lock_guard<std::mutex> lk(c_esp::geometry_mtx);
                c_esp::geometry.clear();
                ready = true;
                building = false;
                return;
            }

            std::vector<uintptr_t> players = core.get_players(g_main::datamodel);
            std::vector<uintptr_t> player_models;
            for (auto& p : players)
            {
                auto m = core.get_model_instance(p);
                if (m) player_models.push_back(m);
            }
   
            std::function<void(uintptr_t)> scan_children = [&](uintptr_t parent)
                {
                    auto children = core.children(parent);
                    for (auto child : children)
                    {
                        if (!child) continue;

                        std::string class_name = core.get_instance_classname(child);

                        if (class_name == "Part" || class_name == "MeshPart" || class_name == "UnionOperation")
                        {
                            bool is_player_part = false;
                            for (auto model : player_models)
                            {
                                if (child == model)
                                {
                                    is_player_part = true;
                                    break;
                                }

                                uintptr_t check = child;
                                while (check != 0)
                                {
                                    uintptr_t parent_check = memory->read<uintptr_t>(check + offsets::Parent);
                                    if (parent_check == model)
                                    {
                                        is_player_part = true;
                                        break;
                                    }
                                    check = parent_check;
                                }
                                if (is_player_part) break;
                            }

                            if (!is_player_part)
                            {
                                uintptr_t primitive = memory->read<uintptr_t>(child + offsets::Primitive);
                                if (primitive >= 0x10000)
                                {
                                    float transparency = memory->read<float>(primitive + offsets::Transparency);
                                    if (transparency <= 0.9f)
                                    {
                                        vector size = memory->read<vector>(primitive + offsets::PartSize);
                                        float vol = size.x * size.y * size.z;
                                        if (vol >= 0.5f && vol <= 8000000.0f)
                                        {
                                            vector pos = memory->read<vector>(primitive + offsets::Position);
                                            parts.push_back({ pos, size, vol, vol > 10.0f });
                                        }
                                    }
                                }
                            }
                        }

                        scan_children(child);
                    }
                };

            scan_children(workspace);

            {
                std::lock_guard<std::mutex> lk(c_esp::geometry_mtx);
                c_esp::geometry = std::move(parts);
            }

            {
                std::lock_guard<std::mutex> lk2(c_esp::vis_cache_mtx);
                c_esp::vis_cache.clear();
            }

            ready = true;
            building = false;

        }).detach();
}

bool c_esp::is_visible(const vector& from, const vector& to, uintptr_t target_model)
{
    return is_visible(from, to, to, to, to, to, target_model);
}

bool c_esp::is_visible(
    const vector& from,
    const vector& head,
    const vector& torso,
    const vector& pelvis,
    const vector& left_foot,
    const vector& right_foot,
    uintptr_t target_model
)
{
    if (!ready) return true;

    std::vector<WorldPart> local_geometry;
    {
        std::lock_guard<std::mutex> lk(geometry_mtx);
        local_geometry = geometry;
    }

    if (local_geometry.empty()) return true;

    if (target_model != 0)
    {
        std::lock_guard<std::mutex> vlk(vis_cache_mtx);
        auto it = vis_cache.find(target_model);
        if (it != vis_cache.end())
        {
            auto age = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - it->second.second).count();
            if (age < 100) return it->second.first;
        }
    }

    struct Ray
    {
        vector target;
        vector dir;
        float dist;
    };

    Ray rays[5] = {
        { head, normalize_vec({ head.x - from.x, head.y - from.y, head.z - from.z }), magnitude_vec({ head.x - from.x, head.y - from.y, head.z - from.z }) },
        { torso, normalize_vec({ torso.x - from.x, torso.y - from.y, torso.z - from.z }), magnitude_vec({ torso.x - from.x, torso.y - from.y, torso.z - from.z }) },
        { pelvis, normalize_vec({ pelvis.x - from.x, pelvis.y - from.y, pelvis.z - from.z }), magnitude_vec({ pelvis.x - from.x, pelvis.y - from.y, pelvis.z - from.z }) },
        { left_foot, normalize_vec({ left_foot.x - from.x, left_foot.y - from.y, left_foot.z - from.z }), magnitude_vec({ left_foot.x - from.x, left_foot.y - from.y, left_foot.z - from.z }) },
        { right_foot, normalize_vec({ right_foot.x - from.x, right_foot.y - from.y, right_foot.z - from.z }), magnitude_vec({ right_foot.x - from.x, right_foot.y - from.y, right_foot.z - from.z }) }
    };

    std::sort(std::begin(rays), std::end(rays), [](const Ray& a, const Ray& b) { return a.dist < b.dist; });

    int visible_count = 0;
    int checked = 0;

    for (const auto& ray : rays)
    {
        if (checked >= 5) break;
        if (ray.dist <= 0.0f)
        {
            checked++;
            continue;
        }

        bool clear = true;
        for (const auto& part : local_geometry)
        {
            if (!part.large && part.vol < 1.0f) continue;
            if (part.vol <= 0.0f || part.size.x <= 0.0f || part.size.y <= 0.0f || part.size.z <= 0.0f) continue;

            if (check_ray_box(from, ray.dir, part, ray.dist))
            {
                float part_dist = magnitude_vec({ part.pos.x - from.x, part.pos.y - from.y, part.pos.z - from.z });
                if (std::fabs(part_dist - ray.dist) < 1.5f) continue;
                clear = false;
                break;
            }
        }

        if (clear && ++visible_count >= 2)
        {
            if (target_model != 0)
            {
                std::lock_guard<std::mutex> vlk(vis_cache_mtx);
                vis_cache[target_model] = { true, std::chrono::steady_clock::now() };
            }
            return true;
        }
        checked++;
    }

    bool result = visible_count >= 2;

    if (target_model != 0)
    {
        std::lock_guard<std::mutex> vlk(vis_cache_mtx);
        vis_cache[target_model] = { result, std::chrono::steady_clock::now() };

        if (vis_cache.size() > 100)
        {
            auto now = std::chrono::steady_clock::now();
            for (auto it = vis_cache.begin(); it != vis_cache.end();)
            {
                if (std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second.second).count() > 1000)
                    it = vis_cache.erase(it);
                else
                    ++it;
            }
        }
    }

    return result;
}

void c_esp::run_players(view_matrix_t viewmatrix)
{
    if (vars::esp::show_fps)
        if (vars::esp::show_fps)
        {
            uintptr_t task_scheduler = memory->read<uintptr_t>(offsets::TaskSchedulerPointer);
            if (task_scheduler)
            {
                // Read the frame delta time (usually around offset 0x1A0-0x1C0)
                // Or read the FPS cap and assume it's close to that
                float fps_cap = memory->read<float>(task_scheduler + offsets::TaskSchedulerMaxFPS);

                std::stringstream ss_fps;
                ss_fps << "FPS: " << std::fixed << std::setprecision(0) << fps_cap;
                draw.outlined_string(ImVec2(10, 10), ss_fps.str().c_str(), ImColor(0, 255, 0, 255), ImColor(0, 0, 0, 255), false);
            }
        }



    std::vector<uintptr_t> players = core.get_players(g_main::datamodel);
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

        float health = memory->read<float>(humanoid + offsets::Health);
        float max_health = memory->read<float>(humanoid + offsets::MaxHealth);

        if (vars::esp::hide_dead && health <= 0.0f)
            continue;

        if (!health || !max_health)
            continue;

        auto player_root = core.find_first_child(model, "HumanoidRootPart");
        if (!player_root)
            continue;

        auto p_player_root = memory->read<uintptr_t>(player_root + offsets::Primitive);
        if (!p_player_root)
            continue;

        auto player_head = core.find_first_child(model, "Head");
        if (!player_head)
            continue;

        auto p_player_head = memory->read<uintptr_t>(player_head + offsets::Primitive);
        if (!p_player_head)
            continue;

        std::string player_name = core.get_instance_name(player);
        if (player_name.empty())
            continue;

        vector w_player_root = memory->read<vector>(p_player_root + offsets::Position);
        vector w_player_head = memory->read<vector>(p_player_head + offsets::Position);

        float hip_height = memory->read<float>(humanoid + offsets::HipHeight);
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
            uintptr_t local_player_character_model = core.find_first_child(
                core.find_first_child_class(g_main::datamodel, "Workspace"),
                core.get_instance_name(g_main::localplayer)
            );
            if (local_player_character_model)
            {
                auto local_player_root = core.find_first_child(local_player_character_model, "HumanoidRootPart");
                if (local_player_root)
                {
                    auto p_local_player_root = memory->read<uintptr_t>(local_player_root + offsets::Primitive);
                    if (p_local_player_root)
                    {
                        vector w_local_player_root = memory->read<vector>(p_local_player_root + offsets::Position);
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
                { "Head", "Torso" },
                { "Torso", "HumanoidRootPart" },
                { "Right Arm", "Torso" },
                { "Left Arm", "Torso" },
                { "Right Leg", "Torso" },
                { "Left Leg", "Torso" }
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
    static auto last_cache_update = std::chrono::high_resolution_clock::now();
    auto now_time = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now_time - last_cache_update).count() >= 5)
    {
        update_world_cache();
        last_cache_update = now_time;
    }

    std::vector<uintptr_t> players;
    {
        std::lock_guard<std::mutex> lock(c_esp::players_mtx);
        players = core.get_players(g_main::datamodel);
    }

    vector2d crosshair_pos = {
        static_cast<float>(core.get_screen_width() / 2),
        static_cast<float>(core.get_screen_height() / 2)
    };

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
                        float health = memory->read<float>(humanoid + offsets::Health);
                        if (health > 0.0f)
                        {
                            const char* target_bone_name = (vars::aimbot::aimbot_hitbox == 0) ? "Head" : "HumanoidRootPart";
                            auto target_bone = core.find_first_child(model, target_bone_name);
                            if (target_bone)
                            {
                                auto p_target_bone = memory->read<uintptr_t>(target_bone + offsets::Primitive);
                                if (p_target_bone)
                                {
                                    auto player_root_part = core.find_first_child(model, "HumanoidRootPart");
                                    if (!player_root_part) break;
                                    auto p_player_root = memory->read<uintptr_t>(player_root_part + offsets::Primitive);
                                    if (!p_player_root) break;
                                    vector w_target_bone_pos = memory->read<vector>(p_target_bone + offsets::Position);
                                    vector v_player_root = memory->read<vector>(p_player_root + offsets::Velocity);
                                
                                    vector2d s_target_bone_pos;
                                    if (core.world_to_screen(w_target_bone_pos, s_target_bone_pos, viewmatrix))
                                    {
                                        float distance = sqrtf(
                                            powf(s_target_bone_pos.x - crosshair_pos.x, 2) +
                                            powf(s_target_bone_pos.y - crosshair_pos.y, 2)
                                        );
                                        if (distance < vars::aimbot::fov)
                                        {
                                            locked_target_still_valid = true;
                                            current_target = this->locked_target;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            }
        }
 
        if (!locked_target_still_valid)
            this->locked_target = 0;
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

            float health = memory->read<float>(humanoid + offsets::Health);
            if (health <= 0.0f)
                continue;

            auto player_root = core.find_first_child(model, "HumanoidRootPart");
            if (!player_root)
                continue;

            auto p_player_root = memory->read<uintptr_t>(player_root + offsets::Primitive);
            if (!p_player_root)
                continue;

            const char* target_bone_name = (vars::aimbot::aimbot_hitbox == 0) ? "Head" : "HumanoidRootPart";
            auto target_bone = core.find_first_child(model, target_bone_name);
            if (!target_bone)
                continue;

            auto p_target_bone = memory->read<uintptr_t>(target_bone + offsets::Primitive);
            if (!p_target_bone)
                continue;

            vector w_target_bone_pos = memory->read<vector>(p_target_bone + offsets::Position);
            vector v_player_root = memory->read<vector>(p_player_root + offsets::Velocity);

            vector2d s_target_bone_pos;
            if (!core.world_to_screen(w_target_bone_pos, s_target_bone_pos, viewmatrix))
                continue;

            float distance = sqrtf(
                powf(s_target_bone_pos.x - crosshair_pos.x, 2) +
                powf(s_target_bone_pos.y - crosshair_pos.y, 2)
            );

            if (distance < search_fov && distance < new_closest_distance)
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
        auto p_player_root = memory->read<uintptr_t>(player_root + offsets::Primitive);
        vector v_player_root = memory->read<vector>(p_player_root + offsets::Velocity);

        const char* target_bone_name = (vars::aimbot::aimbot_hitbox == 0) ? "Head" : "HumanoidRootPart";
        auto target_bone = core.find_first_child(model, target_bone_name);
        auto p_target_bone = memory->read<uintptr_t>(target_bone + offsets::Primitive);
        vector w_target_bone_pos = memory->read<vector>(p_target_bone + offsets::Position);

        if (vars::aimbot::prediction)
        {
            w_target_bone_pos.x += v_player_root.x * 0.05f;  // Half the original time
            w_target_bone_pos.y += v_player_root.y * 0.05f;
            w_target_bone_pos.z += v_player_root.z * 0.05f;
        }

        vector2d s_target_bone_pos;
        if (core.world_to_screen(w_target_bone_pos, s_target_bone_pos, viewmatrix))
        {
            float delta_x = s_target_bone_pos.x - crosshair_pos.x;
            float delta_y = s_target_bone_pos.y - crosshair_pos.y;

            if (triggerbot_active)
            {
                float distance = sqrtf(delta_x * delta_x + delta_y * delta_y);
                if (distance < vars::triggerbot::fov)
                {
                    vector w_player_root = memory->read<vector>(p_player_root + offsets::Position);
                    vector w_head = w_target_bone_pos;
                    vector w_pelvis = w_player_root;
                    vector w_torso = w_player_root;
                    vector w_left_foot = w_player_root;
                    vector w_right_foot = w_player_root;

                    auto torso_part = core.find_first_child(model, "Torso");
                    if (torso_part)
                    {
                        auto p_torso = memory->read<uintptr_t>(torso_part + offsets::Primitive);
                        if (p_torso) w_torso = memory->read<vector>(p_torso + offsets::Position);
                    }

                    auto left_leg = core.find_first_child(model, "Left Leg");
                    if (left_leg)
                    {
                        auto p_left = memory->read<uintptr_t>(left_leg + offsets::Primitive);
                        if (p_left) w_left_foot = memory->read<vector>(p_left + offsets::Position);
                    }

                    auto right_leg = core.find_first_child(model, "Right Leg");
                    if (right_leg)
                    {
                        auto p_right = memory->read<uintptr_t>(right_leg + offsets::Primitive);
                        if (p_right) w_right_foot = memory->read<vector>(p_right + offsets::Position);
                    }

                    if (camera && !is_visible(cam_pos, w_head, w_torso, w_pelvis, w_left_foot, w_right_foot, model))
                    {
                        trigger_waiting = false;
                    }
                    else
                    {
                        auto now = std::chrono::steady_clock::now();
                        if (!trigger_waiting)
                        {
                            trigger_fire_time = now + std::chrono::milliseconds(vars::triggerbot::delay);
                            trigger_waiting = true;
                        }

                        if (now >= trigger_fire_time)
                        {
                            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                            auto release_time = now + std::chrono::milliseconds(vars::triggerbot::hold_time);
                            while (std::chrono::steady_clock::now() < release_time) {}
                            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                            trigger_waiting = false;
                        }
                    }
                }
                else
                {
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
                    if (move_x == current_mouse_pos.x && target_x != current_mouse_pos.x)
                        move_x += (target_x > current_mouse_pos.x ? 1 : -1);
                    if (move_y == current_mouse_pos.y && target_y != current_mouse_pos.y)
                        move_y += (target_y > current_mouse_pos.y ? 1 : -1);
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
                    INPUT input = {};
                    input.type = INPUT_MOUSE;
                    input.mi.dx = move_x;
                    input.mi.dy = move_y;
                    input.mi.dwFlags = MOUSEEVENTF_MOVE;
                    SendInput(1, &input, sizeof(INPUT));
                }
            }
        }
        else
        {
            this->leftover_x = 0.0f;
            this->leftover_y = 0.0f;
            this->smoothed_delta_x = 0.0f;
            this->smoothed_delta_y = 0.0f;
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

void c_esp::draw_hitbox_esp(view_matrix_t viewmatrix)
{
    if (!vars::combat::hitbox_expander || !vars::combat::hitbox_visible)
        return;

    if (!g_main::datamodel || !g_main::localplayer)
        return;

    std::vector<uintptr_t> players = core.get_players(g_main::datamodel);

    for (uintptr_t player : players)
    {
        if (!player || player == g_main::localplayer)
            continue;

        if (vars::combat::hitbox_skip_teammates)
        {
            uintptr_t player_team = memory->read<uintptr_t>(player + offsets::Team);
            if (player_team != 0 && player_team == g_main::localplayer_team)
                continue;
        }

        uintptr_t character = core.get_model_instance(player);
        if (!character) continue;

        uintptr_t hrp = core.find_first_child(character, "HumanoidRootPart");
        if (!hrp) continue;

        uintptr_t primitive = memory->read<uintptr_t>(hrp + offsets::Primitive);
        if (!primitive) continue;

        // Get HRP position
        vector hrp_pos = memory->read<vector>(primitive + offsets::Position);

        // Get hitbox size
        float size_x = vars::combat::hitbox_size_x;
        float size_y = vars::combat::hitbox_size_y;
        float size_z = vars::combat::hitbox_size_z;

        // Calculate 8 corners of the hitbox
        vector corners[8] = {
            { hrp_pos.x - size_x/2, hrp_pos.y - size_y/2, hrp_pos.z - size_z/2 },
            { hrp_pos.x + size_x/2, hrp_pos.y - size_y/2, hrp_pos.z - size_z/2 },
            { hrp_pos.x + size_x/2, hrp_pos.y + size_y/2, hrp_pos.z - size_z/2 },
            { hrp_pos.x - size_x/2, hrp_pos.y + size_y/2, hrp_pos.z - size_z/2 },
            { hrp_pos.x - size_x/2, hrp_pos.y - size_y/2, hrp_pos.z + size_z/2 },
            { hrp_pos.x + size_x/2, hrp_pos.y - size_y/2, hrp_pos.z + size_z/2 },
            { hrp_pos.x + size_x/2, hrp_pos.y + size_y/2, hrp_pos.z + size_z/2 },
            { hrp_pos.x - size_x/2, hrp_pos.y + size_y/2, hrp_pos.z + size_z/2 }
        };

        // Convert to screen coordinates
        vector2d screen_corners[8];
        bool all_visible = true;

        for (int i = 0; i < 8; i++)
        {
            if (!core.world_to_screen(corners[i], screen_corners[i], viewmatrix))
            {
                all_visible = false;
                break;
            }
        }

        if (!all_visible) continue;

        ImColor hitbox_color = ImColor(255, 0, 0, 150);  // Red, semi-transparent

        // Draw 12 edges of the box
        // Bottom face
        draw.line(ImVec2(screen_corners[0].x, screen_corners[0].y), ImVec2(screen_corners[1].x, screen_corners[1].y), hitbox_color, 1.0f);
        draw.line(ImVec2(screen_corners[1].x, screen_corners[1].y), ImVec2(screen_corners[2].x, screen_corners[2].y), hitbox_color, 1.0f);
        draw.line(ImVec2(screen_corners[2].x, screen_corners[2].y), ImVec2(screen_corners[3].x, screen_corners[3].y), hitbox_color, 1.0f);
        draw.line(ImVec2(screen_corners[3].x, screen_corners[3].y), ImVec2(screen_corners[0].x, screen_corners[0].y), hitbox_color, 1.0f);

        // Top face
        draw.line(ImVec2(screen_corners[4].x, screen_corners[4].y), ImVec2(screen_corners[5].x, screen_corners[5].y), hitbox_color, 1.0f);
        draw.line(ImVec2(screen_corners[5].x, screen_corners[5].y), ImVec2(screen_corners[6].x, screen_corners[6].y), hitbox_color, 1.0f);
        draw.line(ImVec2(screen_corners[6].x, screen_corners[6].y), ImVec2(screen_corners[7].x, screen_corners[7].y), hitbox_color, 1.0f);
        draw.line(ImVec2(screen_corners[7].x, screen_corners[7].y), ImVec2(screen_corners[4].x, screen_corners[4].y), hitbox_color, 1.0f);

        // Vertical edges
        draw.line(ImVec2(screen_corners[0].x, screen_corners[0].y), ImVec2(screen_corners[4].x, screen_corners[4].y), hitbox_color, 1.0f);
        draw.line(ImVec2(screen_corners[1].x, screen_corners[1].y), ImVec2(screen_corners[5].x, screen_corners[5].y), hitbox_color, 1.0f);
        draw.line(ImVec2(screen_corners[2].x, screen_corners[2].y), ImVec2(screen_corners[6].x, screen_corners[6].y), hitbox_color, 1.0f);
        draw.line(ImVec2(screen_corners[3].x, screen_corners[3].y), ImVec2(screen_corners[7].x, screen_corners[7].y), hitbox_color, 1.0f);
    }
}

void c_esp::start_hitbox_thread() {
    if (!c_esp::hitbox_thread_running) {
        std::thread(c_esp::hitbox_expander_thread).detach();
        c_esp::hitbox_thread_running = true;
    }
}

static bool is_valid_pointer(uintptr_t ptr)
{
    if (!ptr) return false;
    if (ptr < 0x10000) return false;
    if (ptr > 0x7FFFFFFFFFFF) return false;
    return true;
}

void c_esp::hitbox_expander_thread()
{
    while (true)
    {
        // Check if enabled
        if (!vars::combat::hitbox_expander)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }

        // Validate globals
        if (!is_valid_pointer(g_main::datamodel))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }

        if (!is_valid_pointer(g_main::localplayer))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }

        try
        {
            // Get players safely
            std::vector<uintptr_t> players;

            try
            {
                players = core.get_players(g_main::datamodel);
            }
            catch (...)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                continue;
            }

            if (players.empty())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            c_esp::hitbox_processed_count = 0;

            // Pre-calculate size
            vector newSize = {
                vars::combat::hitbox_size_x,
                vars::combat::hitbox_size_y,
                vars::combat::hitbox_size_z
            };

            // Validate size values
            if (newSize.x <= 0.0f || newSize.x > 1000.0f) continue;
            if (newSize.y <= 0.0f || newSize.y > 1000.0f) continue;
            if (newSize.z <= 0.0f || newSize.z > 1000.0f) continue;

            for (uintptr_t player : players)
            {
                // Validate player pointer
                if (!is_valid_pointer(player)) continue;
                if (player == g_main::localplayer) continue;

                // Skip teammates
                if (vars::combat::hitbox_skip_teammates)
                {
                    try
                    {
                        uintptr_t player_team = memory->read<uintptr_t>(player + offsets::Team);
                        if (player_team != 0 && player_team == g_main::localplayer_team)
                            continue;
                    }
                    catch (...)
                    {
                        continue;
                    }
                }

                // Get character
                uintptr_t character = 0;
                try
                {
                    character = core.get_model_instance(player);
                }
                catch (...)
                {
                    continue;
                }

                if (!is_valid_pointer(character)) continue;

                // Get HumanoidRootPart
                uintptr_t hrp = 0;
                try
                {
                    hrp = core.find_first_child(character, "HumanoidRootPart");
                }
                catch (...)
                {
                    continue;
                }

                if (!is_valid_pointer(hrp)) continue;

                // Get primitive
                uintptr_t primitive = 0;
                try
                {
                    primitive = memory->read<uintptr_t>(hrp + offsets::Primitive);
                }
                catch (...)
                {
                    continue;
                }

                if (!is_valid_pointer(primitive)) continue;

                // Validate by reading current size first
                try
                {
                    vector currentSize = memory->read<vector>(primitive + offsets::PartSize);

                    // Check if current size is reasonable (not garbage memory)
                    if (currentSize.x <= 0.0f || currentSize.x > 1000.0f) continue;
                    if (currentSize.y <= 0.0f || currentSize.y > 1000.0f) continue;
                    if (currentSize.z <= 0.0f || currentSize.z > 1000.0f) continue;
                }
                catch (...)
                {
                    continue;
                }

                // Write new size
                try
                {
                    memory->write<vector>(primitive + offsets::PartSize, newSize);
                    c_esp::hitbox_processed_count++;
                }
                catch (...)
                {
                    continue;
                }

                // Small delay between players to reduce stress
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
        catch (...)
        {
            // Major error - wait longer
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        // Normal delay between loops
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}