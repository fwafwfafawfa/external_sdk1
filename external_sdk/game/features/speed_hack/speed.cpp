#include "speed.hpp"
#include "../../../handlers/vars.hpp"
#include "../../../game/offsets/offsets.hpp"
#include "../../../game/core.hpp"
#include "../../../addons/kernel/memory.hpp"
#include "../../../handlers/utility/utility.hpp"

namespace speed_hack
{
    static bool last_toggled_state = false;
    static float last_speed_value = 16.0f; // Initialize with default

    void run()
    {
        uintptr_t humanoid_ptr = core.get_local_humanoid();
        if (!humanoid_ptr)
            return;

        bool current_toggled = vars::speed_hack::toggled;
        float current_value = vars::speed_hack::value;

        if (current_toggled)
        {
            memory->write<float>(humanoid_ptr + offsets::WalkSpeed, current_value);
            memory->write<float>(humanoid_ptr + offsets::WalkSpeedCheck, current_value);
        }
        else if (last_toggled_state)
        {
            memory->write<float>(humanoid_ptr + offsets::WalkSpeed, 16.0f);
            memory->write<float>(humanoid_ptr + offsets::WalkSpeedCheck, 16.0f);
        }

        last_toggled_state = current_toggled;
        last_speed_value = current_value;
    }
}