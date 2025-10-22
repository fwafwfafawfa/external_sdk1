#include "misc.hpp"
#include <iostream>
#include <cmath> // For atan2f

// Define M_PI manually if not provided by cmath
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#include "../../addons/kernel/memory.hpp"

void c_misc::teleport_to(uintptr_t player_instance)
{
    util.m_print("Teleport to Player: Attempting to teleport to 0x%llX", player_instance);

    // Get target player's character model
    auto target_model = core.get_model_instance(player_instance);
    if (!target_model)
    {
        util.m_print("Teleport to Player: Failed to get target model for 0x%llX.", player_instance);
        return;
    }

    // Get target player's HumanoidRootPart position
    auto target_root = core.find_first_child(target_model, "HumanoidRootPart");
    if (!target_root)
    {
        util.m_print("Teleport to Player: Failed to get target HumanoidRootPart for model 0x%llX.", target_model);
        return;
    }
    auto p_target_root = memory->read<uintptr_t>(target_root + offsets::Primitive);
    if (!p_target_root)
    {
        util.m_print("Teleport to Player: Failed to get target Primitive for HumanoidRootPart 0x%llX.", target_root);
        return;
    }
    vector w_target_pos_raw = memory->read<vector>(p_target_root + offsets::Position);
    util.m_print("Teleport to Player: Raw target position: (%.1f, %.1f, %.1f)", w_target_pos_raw.x, w_target_pos_raw.y, w_target_pos_raw.z);

    // Apply configurable offsets
    vector w_target_pos_final = w_target_pos_raw;
    w_target_pos_final.y += vars::misc::teleport_offset_y;
    w_target_pos_final.z += vars::misc::teleport_offset_z;
    util.m_print("Teleport to Player: Final target position (with offsets): (%.1f, %.1f, %.1f)", w_target_pos_final.x, w_target_pos_final.y, w_target_pos_final.z);

    // Validate final target position
    if (!std::isfinite(w_target_pos_final.x) || !std::isfinite(w_target_pos_final.y) || !std::isfinite(w_target_pos_final.z)) {
        util.m_print("Teleport to Player: Aborting teleport due to invalid (NaN/Infinity) final target position.");
        return;
    }

    // Call the more reliable teleport_to_position function
    // We need to find p_local_root here as well
    uintptr_t local_player_character_model = core.find_first_child(core.find_first_child_class(g_main::datamodel, "Workspace"), core.get_instance_name(g_main::localplayer));
    if (!local_player_character_model)
    {
        util.m_print("Teleport to Player: ERROR - Local player character model not found for teleport_to.");
        return;
    }
    auto local_root = core.find_first_child(local_player_character_model, "HumanoidRootPart");
    if (!local_root)
    {
        util.m_print("Teleport to Player: ERROR - Local HumanoidRootPart not found for teleport_to.");
        return;
    }
    auto p_local_root = memory->read<uintptr_t>(local_root + offsets::Primitive);
    if (!p_local_root)
    {
        util.m_print("Teleport to Player: ERROR - Local HumanoidRootPart Primitive is NULL for teleport_to.");
        return;
    }

    teleport_to_position(p_local_root, w_target_pos_final);
    util.m_print("Teleport to Player: Teleported local player to (%.1f, %.1f, %.1f). WARNING: HIGH DETECTION RISK!", w_target_pos_final.x, w_target_pos_final.y, w_target_pos_final.z);
}

void c_misc::teleport_to_position(uintptr_t p_local_root, vector target_pos)
{
    if (!p_local_root)
    {
        util.m_print("Teleport to Position: ERROR - p_local_root is NULL.");
        return;
    }

    util.m_print("Teleport to Position: Attempting to write to 0x%llX with target position (%.1f, %.1f, %.1f)", p_local_root + offsets::Position, target_pos.x, target_pos.y, target_pos.z);

    // Sanity check for target position before writing
    if (!std::isfinite(target_pos.x) || !std::isfinite(target_pos.y) || !std::isfinite(target_pos.z)) {
        util.m_print("Teleport to Position: ERROR - Aborting write due to invalid (NaN/Infinity) target position.");
        return;
    }

    // Write target position to local player's position
    memory->write<vector>(p_local_root + offsets::Position, target_pos);
}

void c_misc::teleport_to_cframe(uintptr_t p_local_root, CFrame target_cframe)
{
    if (!p_local_root)
    {
        util.m_print("Teleport to CFrame: ERROR - p_local_root is NULL.");
        return;
    }

    // Launch a new thread to repeatedly write the CFrame
    std::thread([=]() {
        util.m_print("Teleport to CFrame: Starting forceful CFrame write loop.");
        for (auto i = 0; i != 100; i++)
        {
            memory->write<CFrame>(p_local_root + offsets::CFrame, target_cframe);
            // Small delay to allow game to process, if needed, but loop is fast enough
        }
        util.m_print("Teleport to CFrame: Forceful CFrame write loop finished.");
    }).detach();
}

void c_misc::spectate(uintptr_t player_instance)
{
    util.m_print("Spectate Player: Attempting to spectate 0x%llX", player_instance);

    // Get target player's character model
    auto target_model = core.get_model_instance(player_instance);
    if (!target_model)
    {
        util.m_print("Spectate Player: Failed to get target model.");
        return;
    }

    // Get target player's HumanoidRootPart position (where we want to place the camera)
    auto target_root = core.find_first_child(target_model, "HumanoidRootPart");
    if (!target_root)
    {
        util.m_print("Spectate Player: Failed to get target HumanoidRootPart.");
        return;
    }
    auto p_target_root = memory->read<uintptr_t>(target_root + offsets::Primitive);
    if (!p_target_root)
    {
        util.m_print("Spectate Player: Failed to get target Primitive.");
        return;
    }
    vector w_target_pos = memory->read<vector>(p_target_root + offsets::Position);

    // Find the local player's camera object
    // This is a common pattern: DataModel -> Workspace -> CurrentCamera
    // Or Player -> PlayerMouse -> Camera
    // Let's try to find the camera object via PlayerMouse
    uintptr_t local_player_obj = g_main::localplayer; // This is the Player object
    if (!local_player_obj)
    {
        util.m_print("Spectate Player: Local player object not found.");
        return;
    }

    uintptr_t player_mouse = memory->read<uintptr_t>(local_player_obj + offsets::PlayerMouse);
    if (!player_mouse)
    {
        util.m_print("Spectate Player: PlayerMouse object not found.");
        return;
    }

    uintptr_t camera_obj = memory->read<uintptr_t>(player_mouse + offsets::Camera);
    if (!camera_obj)
    {
        util.m_print("Spectate Player: Camera object not found.");
        return;
    }
    util.m_print("Spectate Player: Camera object found at 0x%llX", camera_obj); // DEBUG

    // Write target position to camera's position
    memory->write<vector>(camera_obj + offsets::CameraPos, w_target_pos);

    // Also attempt to set camera rotation to look at the target
    // This is a simplified calculation, assuming target is at eye level
    vector current_cam_pos = memory->read<vector>(camera_obj + offsets::CameraPos);
    util.m_print("Spectate Debug: Current Camera Pos: (%.1f, %.1f, %.1f)", current_cam_pos.x, current_cam_pos.y, current_cam_pos.z); // DEBUG
    util.m_print("Spectate Debug: Target Pos: (%.1f, %.1f, %.1f)", w_target_pos.x, w_target_pos.y, w_target_pos.z); // DEBUG

    vector look_dir = {w_target_pos.x - current_cam_pos.x, w_target_pos.y - current_cam_pos.y, w_target_pos.z - current_cam_pos.z};
    util.m_print("Spectate Debug: Look Dir: (%.1f, %.1f, %.1f)", look_dir.x, look_dir.y, look_dir.z); // DEBUG

    float yaw = atan2f(look_dir.z, look_dir.x) * (180.0f / M_PI);
    float pitch = atan2f(look_dir.y, sqrtf(look_dir.x * look_dir.x + look_dir.z * look_dir.z)) * (180.0f / M_PI);

    vector cam_rot = {pitch, yaw, 0.0f}; // Assuming X is Pitch, Y is Yaw, Z is Roll (0)
    memory->write<vector>(camera_obj + offsets::CameraRotation, cam_rot);

    util.m_print("Spectate Player: Moved camera to (%.1f, %.1f, %.1f) with rotation (%.1f, %.1f, %.1f). WARNING: HIGH DETECTION RISK!", w_target_pos.x, w_target_pos.y, w_target_pos.z, cam_rot.x, cam_rot.y, cam_rot.z);
}

static std::chrono::steady_clock::time_point last_afk_action_time = std::chrono::steady_clock::now();

void c_misc::run_anti_afk()
{
    static std::atomic<bool> running{ false };

    if (running)
        return;

    running = true;

    std::thread([]()
        {
            util.m_print("antiafking");

            while (vars::anti_afk::toggled)
            {
                uintptr_t localplayer = g_main::localplayer;
                if (!localplayer)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    continue;
                }

                uintptr_t playerconfig = memory->read<uintptr_t>(localplayer + offsets::PlayerConfigurerPointer);
                if (!playerconfig)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    continue;
                }
                memory->write<int>(memory->find_image() + 0x5CFF4F4, 99999);
                memory->write<int>(playerconfig + offsets::ForceNewAFKDuration, 1);

                std::this_thread::sleep_for(std::chrono::seconds(5));
            }

            util.m_print("no more antiafk");
            running = false;
        }).detach();
}
