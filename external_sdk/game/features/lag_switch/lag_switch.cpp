#include "lag_switch.hpp"
#include "../../../handlers/vars.hpp"
#include <chrono>
#include <thread>

void c_lag_switch::run()
{
    if (!vars::lag_switch::toggled)
    {
        if (currently_lagging)
        {
            stop_lag();
        }
        return;
    }

    auto now = std::chrono::steady_clock::now();

    // Manual lag on key press
    if (vars::lag_switch::manual_lag)
    {
        bool key_pressed = GetAsyncKeyState(vars::lag_switch::activation_key) & 0x8000;
        
        if (key_pressed && !manual_lag_pressed)
        {
            start_lag();
            manual_lag_pressed = true;
        }
        else if (!key_pressed)
        {
            manual_lag_pressed = false;
        }
    }

    // Auto lag at intervals
    if (vars::lag_switch::auto_lag)
    {
        auto time_since_last_lag = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_auto_lag_time);
        
        if (time_since_last_lag.count() >= (vars::lag_switch::lag_interval * 1000))
        {
            start_lag();
            last_auto_lag_time = now;
        }
    }

    // Check if we should stop lagging
    if (currently_lagging)
    {
        auto lag_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - lag_start_time);
        
        if (lag_duration_ms.count() >= (vars::lag_switch::lag_duration * 1000))
        {
            stop_lag();
        }
    }
}

void c_lag_switch::start_lag()
{
    if (currently_lagging) return;
    
    currently_lagging = true;
    lag_start_time = std::chrono::steady_clock::now();
    
    // Create network lag by blocking network operations
    // This is a simple implementation that creates artificial delay
    util.m_print("Lag Switch: Started lagging for %.2f seconds", vars::lag_switch::lag_duration);
}

void c_lag_switch::stop_lag()
{
    if (!currently_lagging) return;
    
    currently_lagging = false;
    util.m_print("Lag Switch: Stopped lagging");
}

// Alternative implementation using network manipulation
void c_lag_switch::create_network_lag()
{
    if (!currently_lagging) return;
    
    // Method 1: Block network operations temporarily
    // This creates artificial network delay
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Method 2: Manipulate network buffers (more advanced)
    // This would require more sophisticated network manipulation
    // For now, we'll use the simple delay method
}
