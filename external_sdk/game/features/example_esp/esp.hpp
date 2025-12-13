#pragma once
#include "../../../main.hpp"

class c_esp
{
public:
    float leftover_x = 0.0f;
    float leftover_y = 0.0f;
    float smoothed_delta_x = 0.0f;
    float smoothed_delta_y = 0.0f;
    uintptr_t locked_target = 0;

    void calculate_fps();
    void run_players(view_matrix_t viewmatrix);
    void run_aimbot(view_matrix_t viewmatrix);
    bool is_visible(const vector& from, const vector& to, uintptr_t target_model);
};

inline c_esp esp;