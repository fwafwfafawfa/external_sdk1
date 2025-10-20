#include "infinite_jump.hpp"
#include "../../../handlers/vars.hpp"
#include "../../../game/offsets/offsets.hpp"
#include "../../../game/core.hpp"
#include "../../../addons/kernel/memory.hpp"
#include "../../../handlers/utility/utility.hpp"

namespace infinite_jump
{
    void run()
    {
        if (!vars::infinite_jump::toggled)
            return;

        uintptr_t humanoid_ptr = core.get_local_humanoid();
        if (!humanoid_ptr)
            return;

        if (GetKeyState(VK_SPACE) & 0x8000)
        {
            int state_id = memory->read<int>(humanoid_ptr + offsets::HumanoidStateId);
            if (state_id != 3) // If not already Jumping
            {
                memory->write<int>(humanoid_ptr + offsets::HumanoidStateId, 3); // Jumping
            }
        }
    }
}

