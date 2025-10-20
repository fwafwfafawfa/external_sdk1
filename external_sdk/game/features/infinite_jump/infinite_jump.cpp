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

        uintptr_t character = core.get_model_instance(g_main::localplayer);
        if (!character)
            return;

        uintptr_t humanoidRootPart = core.find_first_child(character, "HumanoidRootPart");
        if (!humanoidRootPart)
            return;

        uintptr_t primitive = memory->read<uintptr_t>(humanoidRootPart + offsets::Primitive);
        if (!primitive)
            return;

        // Ensure CanCollide is false and Anchored is false for the primitive
        memory->write<bool>(primitive + offsets::CanCollide, false);
        char primitive_flags = memory->read<char>(primitive + offsets::Anchored);
        if ((primitive_flags & offsets::AnchoredMask) != 0) {
            primitive_flags &= ~offsets::AnchoredMask; // Unset the anchored bit
            memory->write<char>(primitive + offsets::Anchored, primitive_flags);
        }

        if (GetKeyState(VK_SPACE) & 0x8000)
        {
            vector current_velocity = memory->read<vector>(primitive + offsets::Velocity);
            current_velocity.y = vars::infinite_jump::jump_power_value;
            memory->write<vector>(primitive + offsets::Velocity, current_velocity);
        }
    }
}


