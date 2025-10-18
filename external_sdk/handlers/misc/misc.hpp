#pragma once

#include "../../game/core.hpp" // Explicitly include core.hpp first
#include "../../main.hpp"
#include "../../addons/kernel/driver.hpp"
#include "../../game/offsets/offsets.hpp"
#include "../utility/utility.hpp"

class c_misc
{
public:
    void teleport_to(uintptr_t player_instance);
    void teleport_to_position(uintptr_t p_local_root, vector target_pos);
    void spectate(uintptr_t player_instance);
    void run_anti_afk(); // Correctly added
};

inline c_misc misc;
