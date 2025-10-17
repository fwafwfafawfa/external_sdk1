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
    void teleport_to_position(vector target_pos); // New method
    void spectate(uintptr_t player_instance);

};

inline c_misc misc;
