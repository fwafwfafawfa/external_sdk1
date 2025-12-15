#pragma once

#include "../../../main.hpp"
#include <mutex>
#include <atomic>
#include <unordered_map>

struct WorldPart
{
    vector pos;
    vector size;
    float vol;
    bool large;
};

struct TargetData
{
    vector last_velocity;
    vector last_position;
    std::chrono::steady_clock::time_point last_update;
};

class c_esp
{
public:
    float leftover_x = 0.0f;
    float leftover_y = 0.0f;
    float smoothed_delta_x = 0.0f;
    float smoothed_delta_y = 0.0f;
    uintptr_t locked_target = 0;

    static std::vector<WorldPart> geometry;
    static double last_refresh;
    static std::atomic<bool> building;
    static std::atomic<bool> ready;
    static std::unordered_map<uintptr_t, std::pair<bool, std::chrono::steady_clock::time_point>> vis_cache;
    static std::mutex geometry_mtx;
    static std::mutex vis_cache_mtx;
    static std::unordered_map<uintptr_t, TargetData> target_tracking;
    static std::mutex tracking_mtx;
    static void hitbox_expander_thread();
    static void start_hitbox_thread();
    static int hitbox_processed_count;  // PUBLIC
    static void apply_hitbox_expander();
    static std::mutex players_mtx;

    void update_world_cache();
    void calculate_fps();
    void run_players(view_matrix_t viewmatrix);
    void run_aimbot(view_matrix_t viewmatrix);
    void draw_hitbox_esp(view_matrix_t viewmatrix);
    void draw_minimap(view_matrix_t viewmatrix);
    bool is_visible(const vector& from, const vector& to, uintptr_t target_model);
    bool is_visible(const vector& from, const vector& head, const vector& torso, const vector& pelvis, const vector& left_foot, const vector& right_foot, uintptr_t target_model);

private:
    static std::atomic<bool> hitbox_thread_running;

};

inline c_esp esp;