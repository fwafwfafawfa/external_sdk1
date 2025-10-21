#include "player_info.hpp"
#include "../../../handlers/vars.hpp"
#include "../../../game/offsets/offsets.hpp"
#include "../../../game/core.hpp"
#include "../../../addons/kernel/memory.hpp"
#include "../../../handlers/utility/utility.hpp"

void c_player_info::draw_player_info(uintptr_t player_instance)
{
    if (player_instance == 0)
    {
        ImGui::Text("No player selected.");
        return;
    }

    ImGui::Text("Player Address: 0x%llX", player_instance);

    std::string player_name = core.get_instance_name(player_instance);
    ImGui::Text("Name: %s", player_name.c_str());

    std::string player_classname = core.get_instance_classname(player_instance);
    ImGui::Text("Class Name: %s", player_classname.c_str());

    // Spectate Button
    std::string spectateID = "Spectate##" + player_name;
    if (ImGui::Button(spectateID.c_str()))
    {
        uintptr_t camera_ptr = memory->read<uintptr_t>(core.find_first_child_class(g_main::datamodel, "Workspace") + offsets::Camera);
        if (camera_ptr)
        {
            if (vars::misc::spectating_player_name == player_name)
            {
                // Stop spectating, revert to local player
                vars::misc::spectating_player_name = "";
                uintptr_t local_humanoid = core.get_local_humanoid();
                if (local_humanoid)
                {
                    memory->write<uintptr_t>(camera_ptr + offsets::CameraSubject, local_humanoid);
                }
            }
            else
            {
                // Start spectating this player
                uintptr_t player_model = core.get_model_instance(player_instance);
                if (player_model)
                {
                    uintptr_t player_humanoid = core.find_first_child_class(player_model, "Humanoid");
                    if (player_humanoid)
                    {
                        memory->write<uintptr_t>(camera_ptr + offsets::CameraSubject, player_humanoid);
                        vars::misc::spectating_player_name = player_name;
                    }
                }
            }
        }
    }
    ImGui::SameLine(); // Place Teleport button on the same line

    // Teleport Button
    std::string teleportID = "Teleport##" + player_name;
    if (ImGui::Button(teleportID.c_str()))
    {
        uintptr_t player_model = core.get_model_instance(player_instance);
        if (player_model)
        {
            uintptr_t player_hrp = core.find_first_child(player_model, "HumanoidRootPart");
            if (player_hrp)
            {
                uintptr_t p_player_hrp = memory->read<uintptr_t>(player_hrp + offsets::Primitive);
                if (p_player_hrp)
                {
                    CFrame target_cframe = memory->read<CFrame>(p_player_hrp + offsets::CFrame);
                    // Apply offsets to the target CFrame position
                    target_cframe.Y += vars::misc::teleport_offset_y;
                    target_cframe.Z += vars::misc::teleport_offset_z;

                    uintptr_t local_player_character_model = core.find_first_child(core.find_first_child_class(g_main::datamodel, "Workspace"), core.get_instance_name(g_main::localplayer));
                    if (local_player_character_model)
                    {
                        uintptr_t local_hrp = core.find_first_child(local_player_character_model, "HumanoidRootPart");
                        if (local_hrp)
                        {
                            uintptr_t p_local_hrp = memory->read<uintptr_t>(local_hrp + offsets::Primitive);
                            if (p_local_hrp)
                            {
                                misc.teleport_to_cframe(p_local_hrp, target_cframe);
                            }
                        }
                    }
                }
            }
        }
    }

    ImGui::Separator();
    ImGui::Text("Character Info:");

    uintptr_t character_model = core.get_model_instance(player_instance);
    if (character_model)
    {
        ImGui::Text("Character Model Address: 0x%llX", character_model);

        uintptr_t humanoid = core.find_first_child_class(character_model, "Humanoid");
        if (humanoid)
        {
            ImGui::Text("Humanoid Address: 0x%llX", humanoid);
            float health = memory->read<float>(humanoid + offsets::Health);
            float max_health = memory->read<float>(humanoid + offsets::MaxHealth);
            ImGui::Text("Health: %.1f / %.1f", health, max_health);
            ImGui::ProgressBar(health / max_health, ImVec2(-FLT_MIN, 0.0f), std::to_string((int)health).c_str());

            float walkspeed = memory->read<float>(humanoid + offsets::Walkspeed);
            ImGui::Text("WalkSpeed: %.1f", walkspeed);

            float jumppower = memory->read<float>(humanoid + offsets::JumpPower);
            ImGui::Text("JumpPower: %.1f", jumppower);

            // Humanoid State
            uintptr_t humanoid_state_ptr = memory->read<uintptr_t>(humanoid + offsets::HumanoidState);
            if (humanoid_state_ptr)
            {
                uintptr_t humanoid_state_id = memory->read<uintptr_t>(humanoid_state_ptr + offsets::HumanoidStateId);
                // This ID needs to be mapped to a readable string, which is game-specific.
                // For now, just display the ID.
                ImGui::Text("Humanoid State ID: %llu", humanoid_state_id);
            }
        }

        uintptr_t hrp = core.find_first_child(character_model, "HumanoidRootPart");
        if (hrp)
        {
            ImGui::Text("HumanoidRootPart Address: 0x%llX", hrp);
            uintptr_t p_hrp = memory->read<uintptr_t>(hrp + offsets::Primitive);
            if (p_hrp)
            {
                vector pos = memory->read<vector>(p_hrp + offsets::Position);
                ImGui::Text("Position: (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
                vector vel = memory->read<vector>(p_hrp + offsets::Velocity);
                ImGui::Text("Velocity: (%.1f, %.1f, %.1f)", vel.x, vel.y, vel.z);
            }
        }
    }
    else
    {
        ImGui::Text("Character model not found.");
    }

    ImGui::Separator();
    ImGui::Text("Inventory (Backpack):");
    uintptr_t backpack = core.find_first_child_class(player_instance, "Backpack");
    if (backpack)
    {
        std::vector<uintptr_t> items = core.children(backpack);
        if (items.empty())
        {
            ImGui::Text("No items in backpack.");
        }
        else
        {
            for (uintptr_t item : items)
            {
                ImGui::Text("- %s [%s]", core.get_instance_name(item).c_str(), core.get_instance_classname(item).c_str());
            }
        }
    }
    else
    {
        ImGui::Text("Backpack not found.");
    }
}