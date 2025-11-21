#include "infinite_jump.hpp"
#include "../../../handlers/vars.hpp"
#include "../../../game/offsets/offsets.hpp"
#include "../../../game/core.hpp"
#include "../../../addons/kernel/memory.hpp"

namespace infinite_jump
{
    bool last_space = false;
    int cooldown = 0;

    void run()
    {
        if (!vars::infinite_jump::toggled)
            return;

        uintptr_t character = core.get_model_instance(g_main::localplayer);
        if (!character)
            return;

        uintptr_t hrp = core.find_first_child(character, "HumanoidRootPart");
        if (!hrp)
            return;

        uintptr_t primitive = memory->read<uintptr_t>(hrp + offsets::Primitive);
        if (!primitive)
            return;

        bool space = (GetKeyState(VK_SPACE) & 0x8000);

        // Anti-fly cooldown
        if (cooldown > 0)
            cooldown--;

        if (space)
        {
            // apply upward force every frame while holding space
            vector vel = memory->read<vector>(primitive + offsets::Velocity);

            vel.y = vars::infinite_jump::jump_power_value;
            memory->write<vector>(primitive + offsets::Velocity, vel);

            cooldown = 4; // 4–6 frames is ideal
        }
        else
        {
            // when space released, stop jump force
            // allow next jump after cooldown hits 0
        }


        last_space = space;
    }
}
