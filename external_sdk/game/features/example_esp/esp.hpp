#pragma once
#include "../../../main.hpp"

class c_esp
{
public:
    void run_players( matrix viewmatrix );
    void run_aimbot( matrix viewmatrix );
private:
    float leftover_x = 0.0f;
    float leftover_y = 0.0f;
    float smoothed_delta_x = 0.0f;
    float smoothed_delta_y = 0.0f;
};

inline c_esp esp;