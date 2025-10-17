#include "jump_power.hpp"
#include "../../../handlers/vars.hpp"
#include "../../../game/offsets/offsets.hpp"
#include "../../../game/core.hpp"
#include "../../../addons/kernel/driver.hpp"

namespace jump_power
{
    static bool last_toggled_state = false;
    static float last_jump_power_value = 50.0f; // Default Roblox JumpPower

    void run()
    {
        uintptr_t humanoid_ptr = core.get_local_humanoid();
        if (!humanoid_ptr)
            return;

        bool current_toggled = vars::jump_power::toggled;
        float current_value = vars::jump_power::value;

        if (current_toggled)
        {
            driver.write<float>(humanoid_ptr + offsets::JumpPower, current_value);
        }
        else if (last_toggled_state) // If hack was ON last frame but is now OFF, reset to default
        {
            driver.write<float>(humanoid_ptr + offsets::JumpPower, 50.0f);
        }

        // Update static variables for next frame's comparison
        last_toggled_state = current_toggled;
        last_jump_power_value = current_value;
    }
}