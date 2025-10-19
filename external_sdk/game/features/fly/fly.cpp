#include "fly.hpp"
#include "../../../handlers/vars.hpp"
#include "../../../game/offsets/offsets.hpp"
#include "../../../game/core.hpp"
#include "../../../addons/kernel/memory.hpp"

// Static variable to track if fly was enabled in the previous frame
static bool fly_toggled = false; // Renamed from was_fly_enabled_last_frame

void c_fly::run()
{
    if (!g_main::localplayer)
        return;

    if (!vars::fly::toggled) // Use vars::fly::toggled for overall enable/disable
        return;

    bool should_fly = false;

    if (vars::fly::fly_mode == 0) // Hold mode
    {
        should_fly = GetAsyncKeyState(vars::fly::fly_toggle_key);
    }
    else if (vars::fly::fly_mode == 1) // Toggle mode
    {
        if (GetAsyncKeyState(vars::fly::fly_toggle_key) & 0x1) // Check for key press
        {
            fly_toggled = !fly_toggled;
        }
        should_fly = fly_toggled;
    }

    if (!should_fly)
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

    uintptr_t workspace = core.find_first_child_class(g_main::datamodel, "Workspace");
    if (!workspace)
        return;

    uintptr_t Camera = memory->read<uintptr_t>(workspace + offsets::Camera);
    if (!Camera)
        return;

    // Vector camPos = memory->read<vector>(Camera + offsets::CameraPos); // Not used in this snippet
    Matrix3 camCFrame = memory->read<Matrix3>(Camera + offsets::CameraRotation); // Assuming Matrix3 is 3x3 rotation

    vector currentPos = memory->read<vector>(primitive + offsets::Position);

    vector lookVector = vector(-camCFrame.data[2], -camCFrame.data[5], -camCFrame.data[8]);
    vector rightVector = vector(camCFrame.data[0], camCFrame.data[3], camCFrame.data[6]);

    vector moveDirection(0, 0, 0);

    // This snippet only shows the Position method (fly_method == 0)
    // We will implement this as the default behavior for now.

    // Forward/Backward (W/S)
    if (GetAsyncKeyState('W') & 0x8000)
    {
        moveDirection = moveDirection + lookVector;
    }
    if (GetAsyncKeyState('S') & 0x8000)
    {
        moveDirection = moveDirection - lookVector;
    }
    if (GetAsyncKeyState('A') & 0x8000)
    {
        moveDirection = moveDirection - rightVector;
    }
    if (GetAsyncKeyState('D') & 0x8000)
    {
        moveDirection = moveDirection + rightVector;
    }
    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
        moveDirection.y += 1.0f;
    }

    if (!moveDirection.IsZero())
    {
        moveDirection.Normalize();
        moveDirection = moveDirection * vars::fly::speed;
    }

    vector newPos = currentPos + moveDirection;
    memory->write<vector>(primitive + offsets::Position, newPos);
    memory->write<bool>(primitive + offsets::CanCollide, false); // Disable collisions
    memory->write<vector>(primitive + offsets::Velocity, vector(0, 0, 0)); // Zero velocity
}