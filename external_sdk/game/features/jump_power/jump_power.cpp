#include "jump_power.hpp"
#include "../../../handlers/vars.hpp"
#include "../../../game/offsets/offsets.hpp"
#include "../../../game/core.hpp"
#include "../../../addons/kernel/memory.hpp"

namespace jump_power
{
    static bool last_toggled_state = false; // Track previous state

    void run()
    {
        uintptr_t humanoid_ptr = core.get_local_humanoid();
        if (!humanoid_ptr)
            return;

        bool current_toggled = vars::jump_power::toggled;

        if (current_toggled)
        {
            if (!last_toggled_state) // Only print when first toggled
            {
                util.m_print("Jump Power: Toggled ON. Setting to %.1f", vars::jump_power::value);
            }
            memory->write<float>(humanoid_ptr + offsets::JumpPower, vars::jump_power::value);
        }
        else if (last_toggled_state) // If it was on, but now is off, reset to default
        {
            memory->write<float>(humanoid_ptr + offsets::JumpPower, vars::jump_power::default_value);
            util.m_print("Jump Power: Toggled OFF. Reset to %.1f", vars::jump_power::default_value);
        }

        last_toggled_state = current_toggled; // Update state for next frame
    }
}