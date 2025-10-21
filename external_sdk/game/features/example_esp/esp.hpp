#pragma once
#include "../../../main.hpp"

class c_esp
{
public:
    void run_players( view_matrix_t viewmatrix );
    void run_aimbot( view_matrix_t viewmatrix );
private:
    float leftover_x = 0.0f;
    float leftover_y = 0.0f;
    float smoothed_delta_x = 0.0f;
    float smoothed_delta_y = 0.0f;
    uintptr_t locked_target = 0; // New: Stores the currently locked target
};

inline c_esp esp;