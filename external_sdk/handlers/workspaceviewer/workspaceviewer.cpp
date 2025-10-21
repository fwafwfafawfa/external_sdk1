#include "workspaceviewer.hpp"
#include <cmath> // For std::isfinite
#include "../misc/misc.hpp" // Include misc.hpp directly



void c_workspace_viewer::draw_selected_instance_highlight() {
    if (selected_instance == 0) return;

    std::string class_name = core.get_instance_classname(selected_instance);

    // Only highlight Parts and Models for now
    if (class_name.find("Part") == std::string::npos && class_name.find("Model") == std::string::npos) {
        return;
    }

    uintptr_t instance_to_highlight = selected_instance;
    vector world_pos = {0,0,0};

    if (class_name.find("Model") != std::string::npos) {
        // If it's a Model, try to find its PrimaryPart or a BasePart child
        uintptr_t primary_part = 0;

        // Try to find HumanoidRootPart within the model
        uintptr_t hrp_in_model = core.find_first_child(selected_instance, "HumanoidRootPart");
        if (hrp_in_model) {
            primary_part = hrp_in_model;
        } else {
            // If no HRP, try to find the first BasePart child
            std::vector<uintptr_t> children = core.children(selected_instance);
            for (uintptr_t child : children) {
                std::string child_class_name = core.get_instance_classname(child);
                if (child_class_name.find("Part") != std::string::npos) { // Check if it's a BasePart
                                primary_part = child;
                    break;
                }
            }
        }

        if (primary_part) {
            instance_to_highlight = primary_part;
        } else {
            // If no primary part found, can't highlight model effectively
            return;
        }
    }

    uintptr_t primitive = memory->read<uintptr_t>(instance_to_highlight + offsets::Primitive);
    if (!primitive) return;

    world_pos = memory->read<vector>(primitive + offsets::Position);

    // Convert world position to screen position
    vector2d screen_pos;
    view_matrix_t view_matrix;

    uintptr_t camera_obj = 0; // Initialize to nullptr

    uintptr_t local_player_obj = g_main::localplayer; // This is the Player object
    if (!local_player_obj) return;

    uintptr_t player_mouse = memory->read<uintptr_t>(local_player_obj + offsets::PlayerMouse);
    if (!player_mouse) return;

    camera_obj = memory->read<uintptr_t>(player_mouse + offsets::Camera);
    if (!camera_obj) return;

    view_matrix = memory->read<view_matrix_t>(g_main::v_engine + offsets::viewmatrix);

    if (core.world_to_screen(world_pos, screen_pos, view_matrix)) {
        float screen_width = core.get_screen_width();
        float screen_height = core.get_screen_height();

        // Use a fixed size for the highlight, as PartSize is unreliable
        float fixed_box_size = 50.0f; // Adjust this value as needed

        ImVec2 top_left = ImVec2(screen_pos.x - fixed_box_size / 2, screen_pos.y - fixed_box_size / 2);
        ImVec2 bottom_right = ImVec2(screen_pos.x + fixed_box_size / 2, screen_pos.y + fixed_box_size / 2);

        draw.outlined_rectangle(top_left, bottom_right, ImColor(255, 255, 0, 255), ImColor(0, 0, 0, 255), 1.0f); // Yellow box, black outline
    }
}

void c_workspace_viewer::draw_properties() {
    ImGui::BeginChild("Properties", ImVec2(0, 0), true);
    ImGui::Text("Properties");
    ImGui::Separator();

    if (selected_instance == 0) {
        ImGui::Text("Select an instance to view its properties.");
        ImGui::EndChild();
        return;
    }

    // Basic Properties
    ImGui::Text("Address: 0x%llX", selected_instance);
    ImGui::Text("Name: %s", core.get_instance_name(selected_instance).c_str());
    ImGui::Text("ClassName: %s", core.get_instance_classname(selected_instance).c_str());

    ImGui::Separator();

    // Common Properties
    bool anchored = memory->read<bool>(selected_instance + offsets::Anchored);
    ImGui::Checkbox("Anchored", &anchored);

    vector position = memory->read<vector>(selected_instance + offsets::Position);
    ImGui::Text("Position: %.3f, %.3f, %.3f", position.x, position.y, position.z);

    std::string class_name = core.get_instance_classname(selected_instance);
    if (class_name == "Player") {
        ImGui::Separator();
        if (ImGui::Button("Teleport to Player")) {
            util.m_print("Teleport: Calling misc.teleport_to for player: 0x%llX", selected_instance);
            misc.teleport_to(selected_instance);
        }
    }
    else if (class_name.find("Part") != std::string::npos || class_name.find("Model") != std::string::npos) {
        ImGui::Separator();
        if (ImGui::Button("Teleport to")) {
            util.m_print("Teleport: Button clicked. Selected instance: 0x%llX", selected_instance);

            // Get local player's character model
            uintptr_t local_player_character_model_temp = core.find_first_child(core.find_first_child_class(g_main::datamodel, "Workspace"), core.get_instance_name(g_main::localplayer));
            if (!local_player_character_model_temp) {
                util.m_print("Teleport: Could not teleport: Local player character model not found.");
                return;
            }
            util.m_print("Teleport: Local player character model found: 0x%llX", local_player_character_model_temp);

            // Get local player's HumanoidRootPart address
            uintptr_t hrp_temp = core.find_first_child(local_player_character_model_temp, "HumanoidRootPart");
            if (!hrp_temp) {
                util.m_print("Teleport: Could not teleport: Local HumanoidRootPart not found.");
                return;
            }
            util.m_print("Teleport: Local HumanoidRootPart found: 0x%llX", hrp_temp);

            uintptr_t p_local_root = memory->read<uintptr_t>(hrp_temp + offsets::Primitive);
            if (!p_local_root) {
                util.m_print("Teleport: Could not teleport: Local HumanoidRootPart Primitive not found.");
                return;
            }
            util.m_print("Teleport: Local HumanoidRootPart Primitive found: 0x%llX", p_local_root);

                        vector w_target_pos = { 0,0,0 }; // Initialize to zero
            bool position_found = false;

            std::string target_class_name = core.get_instance_classname(selected_instance);

            if (target_class_name.find("Model") != std::string::npos) {
                // If it's a Model, try to find a HumanoidRootPart or a BasePart child
                uintptr_t primary_part = 0; // Placeholder for PrimaryPart logic if offset is found

                // Try to find HumanoidRootPart within the model
                uintptr_t hrp_in_model = core.find_first_child(selected_instance, "HumanoidRootPart");
                if (hrp_in_model) {
                    primary_part = hrp_in_model;
                }
                else {
                    // If no HRP, try to find the first BasePart child
                    std::vector<uintptr_t> children = core.children(selected_instance);
                    for (uintptr_t child : children) {
                        std::string child_class_name = core.get_instance_classname(child);
                        if (child_class_name.find("Part") != std::string::npos) { // Check if it's a BasePart
                            primary_part = child;
                            break;
                        }
                    }
                }

                if (primary_part) {
                    uintptr_t primary_part_primitive = memory->read<uintptr_t>(primary_part + offsets::Primitive);
                    if (primary_part_primitive) {
                        w_target_pos = memory->read<vector>(primary_part_primitive + offsets::Position);
                        position_found = true;
                    }
                }
            }
            else if (target_class_name.find("Part") != std::string::npos) {
                // If it's a Part (or subclass like BasePart), use its Primitive
                uintptr_t target_primitive = memory->read<uintptr_t>(selected_instance + offsets::Primitive);
                if (target_primitive) {
                    w_target_pos = memory->read<vector>(target_primitive + offsets::Position);
                    position_found = true;
                }
            }

            if (!position_found) {
                util.m_print("Teleport: Could not get target position from selected instance (unsupported type or no valid part found).");
                return; // Cannot get position, so return
            }

            // Add a small offset on Y axis to avoid getting stuck
            w_target_pos.y += 5.0f;

            // Sanity check for target position
            if (!std::isfinite(w_target_pos.x) || !std::isfinite(w_target_pos.y) || !std::isfinite(w_target_pos.z)) {
                util.m_print("Teleport: Aborting teleport due to invalid (NaN/Infinity) target position: (%.1f, %.1f, %.1f)", w_target_pos.x, w_target_pos.y, w_target_pos.z);
                return;
            }

            // Teleport using misc.teleport_to_position
            misc.teleport_to_position(p_local_root, w_target_pos);

        }
    }

    ImGui::EndChild();
}

void c_workspace_viewer::draw_instance_node(uintptr_t instance) {
    if (instance == 0) return;

    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (selected_instance == instance) {
        node_flags |= ImGuiTreeNodeFlags_Selected;
    }

    std::string name = core.get_instance_name(instance);
    std::string class_name = core.get_instance_classname(instance);
    std::string label = name + " [" + class_name + "]";

    bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)instance, node_flags, "%s", label.c_str());

    if (ImGui::IsItemClicked()) {
        selected_instance = instance;
    }

    if (node_open) {
        std::vector<uintptr_t> children = core.children(instance);
        for (uintptr_t child : children) {
            draw_instance_node(child);
        }
        ImGui::TreePop();
    }
}

void c_workspace_viewer::run() {

    ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Workspace Viewer", &vars::misc::show_workspace_viewer)) {
        ImGui::Columns(2);

        // Left Pane: Tree View
        ImGui::BeginChild("Tree");
        if (g_main::datamodel) {
            draw_instance_node(g_main::datamodel);
        }
        ImGui::EndChild();

        // Right Pane: Properties
        ImGui::NextColumn();
        draw_properties();

        ImGui::Columns(1);
    }
    ImGui::End();
}

c_workspace_viewer workspace_viewer;