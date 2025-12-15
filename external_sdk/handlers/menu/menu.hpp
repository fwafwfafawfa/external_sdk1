#pragma once
#include "../../main.hpp"
#include "game/features/example_esp/esp.hpp"

class c_menu
{
public:
    void run_main_window( );
    void debug_element( );
private:
    void setup_main_window( );
    bool is_initialized = false;
    bool awaiting_key_press = false;
};

extern c_menu menu;