#pragma once

#include "../../../main.hpp"

class c_lag_switch
{
public:
    void run();
    void start_lag();
    void stop_lag();
    bool is_lagging() const { return currently_lagging; }

private:
    bool currently_lagging = false;
    std::chrono::steady_clock::time_point lag_start_time;
    std::chrono::steady_clock::time_point last_auto_lag_time;
    bool manual_lag_pressed = false;
};

inline c_lag_switch lag_switch;
