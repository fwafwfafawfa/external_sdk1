#include "fly.hpp"
#include "../../../handlers/vars.hpp"
#include "../../../game/offsets/offsets.hpp"
#include "../../../game/core.hpp"
#include "../../../addons/kernel/memory.hpp"

// Static variable to track if fly was enabled in the previous frame
static bool was_fly_enabled_last_frame = false;

void c_fly::run() {
    uintptr_t local_humanoid = core.get_local_humanoid();
    if (!local_humanoid) {
        if (was_fly_enabled_last_frame) {
            was_fly_enabled_last_frame = false;
        }
        return;
    }

    uintptr_t local_player_character_model = core.find_first_child(core.find_first_child_class(g_main::datamodel, "Workspace"), core.get_instance_name(g_main::localplayer));
    if (!local_player_character_model) {
        if (was_fly_enabled_last_frame) {
            was_fly_enabled_last_frame = false;
        }
        return;
    }

    uintptr_t hrp = core.find_first_child(local_player_character_model, "HumanoidRootPart");
    if (!hrp) {
        if (was_fly_enabled_last_frame) {
            was_fly_enabled_last_frame = false;
        }
        return;
    }

    uintptr_t hrp_primitive = memory->read<uintptr_t>(hrp + offsets::Primitive);
    if (!hrp_primitive) {
        if (was_fly_enabled_last_frame) {
            was_fly_enabled_last_frame = false;
        }
        return;
    }

    if (!vars::fly::toggled) {
        // If fly is now disabled, reset velocity
        if (was_fly_enabled_last_frame) {
            memory->write<vector>(hrp_primitive + offsets::AssemblyLinearVelocity, {0.0f, 0.0f, 0.0f});
            memory->write<vector>(hrp_primitive + offsets::AssemblyAngularVelocity, {0.0f, 0.0f, 0.0f});
            was_fly_enabled_last_frame = false;
        }
        return;
    }

    // If fly is enabled
    was_fly_enabled_last_frame = true;

    // Ensure HRP is not anchored for movement
    BYTE anchored_val = memory->read<BYTE>(hrp_primitive + offsets::Anchored);
    if ((anchored_val & 0x02) != 0) { // Check if Anchored bit is set
        anchored_val &= ~0x02; // Unset Anchored bit
        memory->write<BYTE>(hrp_primitive + offsets::Anchored, anchored_val);
    }

    // Read camera CFrame for movement direction
    uintptr_t workspace = core.find_first_child_class(g_main::datamodel, "Workspace");
    if (!workspace) return;
    uintptr_t camera_ptr = memory->read<uintptr_t>(workspace + offsets::Camera);
    if (!camera_ptr) return;
    matrix camera_cframe = memory->read<matrix>(camera_ptr + offsets::CFrame);

    vector current_position = memory->read<vector>(hrp_primitive + offsets::Position);
    vector move_direction = {0.0f, 0.0f, 0.0f};
    float speed = vars::fly::speed;

    // Forward/Backward (W/S)
    if (GetAsyncKeyState('W') & 0x8000) {
        move_direction.x -= camera_cframe.m[0][2];
        move_direction.y -= camera_cframe.m[1][2];
        move_direction.z -= camera_cframe.m[2][2];
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        move_direction.x += camera_cframe.m[0][2];
        move_direction.y += camera_cframe.m[1][2];
        move_direction.z += camera_cframe.m[2][2];
    }

    // Left/Right (A/D)
    if (GetAsyncKeyState('A') & 0x8000) {
        move_direction.x -= camera_cframe.m[0][0];
        move_direction.y -= camera_cframe.m[1][0];
        move_direction.z -= camera_cframe.m[2][0];
    }
    if (GetAsyncKeyState('D') & 0x8000) {
        move_direction.x += camera_cframe.m[0][0];
        move_direction.y += camera_cframe.m[1][0];
        move_direction.z += camera_cframe.m[2][0];
    }

    // Normalize move_direction if there's horizontal movement
    if (move_direction.x != 0.0f || move_direction.z != 0.0f) {
        float magnitude = sqrtf(move_direction.x * move_direction.x + move_direction.y * move_direction.y + move_direction.z * move_direction.z);
        if (magnitude > 0) {
            move_direction.x /= magnitude;
            move_direction.y /= magnitude;
            move_direction.z /= magnitude;
        }
    }

    // Apply speed to horizontal movement
    current_position.x += move_direction.x * speed;
    current_position.y += move_direction.y * speed;
    current_position.z += move_direction.z * speed;

    // Up/Down (Space/Control)
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        current_position.y += speed;
    }
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
        current_position.y -= speed;
    }

    // Set LinearVelocity to zero to prevent physics interference
    memory->write<vector>(hrp_primitive + offsets::AssemblyLinearVelocity, {0.0f, 0.0f, 0.0f});
    memory->write<vector>(hrp_primitive + offsets::AssemblyAngularVelocity, {0.0f, 0.0f, 0.0f});

    // Write the new position
    memory->write<vector>(hrp_primitive + offsets::Position, current_position);
}