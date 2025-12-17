// workspaceviewer.cpp
#include "workspaceviewer.hpp"
#include <cmath>
#include "../misc/misc.hpp"

void c_workspace_viewer::draw_selected_instance_highlight() {
    if (selected_instance == 0) return;

    std::string class_name = core.get_instance_classname(selected_instance);

    if (class_name.find("Part") == std::string::npos && class_name.find("Model") == std::string::npos) {
        return;
    }

    uintptr_t instance_to_highlight = selected_instance;
    vector world_pos = { 0,0,0 };

    if (class_name.find("Model") != std::string::npos) {
        uintptr_t primary_part = 0;

        uintptr_t hrp_in_model = core.find_first_child(selected_instance, "HumanoidRootPart");
        if (hrp_in_model) {
            primary_part = hrp_in_model;
        }
        else {
            std::vector<uintptr_t> children = core.children(selected_instance);
            for (uintptr_t child : children) {
                std::string child_class_name = core.get_instance_classname(child);
                if (child_class_name.find("Part") != std::string::npos) {
                    primary_part = child;
                    break;
                }
            }
        }

        if (primary_part) {
            instance_to_highlight = primary_part;
        }
        else {
            return;
        }
    }

    uintptr_t primitive = memory->read<uintptr_t>(instance_to_highlight + offsets::Primitive);
    if (!primitive) return;

    world_pos = memory->read<vector>(primitive + offsets::Position);

    vector2d screen_pos;
    view_matrix_t view_matrix;

    uintptr_t camera_obj = 0;

    uintptr_t local_player_obj = g_main::localplayer;
    if (!local_player_obj) return;

    uintptr_t player_mouse = memory->read<uintptr_t>(local_player_obj + offsets::PlayerMouse);
    if (!player_mouse) return;

    camera_obj = memory->read<uintptr_t>(player_mouse + offsets::Camera);
    if (!camera_obj) return;

    view_matrix = memory->read<view_matrix_t>(g_main::v_engine + offsets::viewmatrix);

    if (core.world_to_screen(world_pos, screen_pos, view_matrix)) {
        float fixed_box_size = 50.0f;

        ImVec2 top_left = ImVec2(screen_pos.x - fixed_box_size / 2, screen_pos.y - fixed_box_size / 2);
        ImVec2 bottom_right = ImVec2(screen_pos.x + fixed_box_size / 2, screen_pos.y + fixed_box_size / 2);

        draw.outlined_rectangle(top_left, bottom_right, ImColor(255, 255, 0, 255), ImColor(0, 0, 0, 255), 1.0f);
    }
}

std::string c_workspace_viewer::dump_script_to_string(uintptr_t script_instance, const std::string& script_type) {
    std::string script_name = core.get_instance_name(script_instance);

    uintptr_t bytecode_offset;
    if (script_type == "LocalScript") {
        bytecode_offset = offsets::LocalScriptByteCode;
    }
    else if (script_type == "ModuleScript") {
        bytecode_offset = offsets::ModuleScriptByteCode;
    }
    else {
        return "Error: Unsupported script type";
    }

    uintptr_t bytecode_string_ptr = memory->read<uintptr_t>(script_instance + bytecode_offset);

    if (!bytecode_string_ptr || bytecode_string_ptr < 0x10000) {
        return "Error: No bytecode string found";
    }

    std::string debug = script_name + " [" + script_type + "]\n\n";
    debug += "String object structure dump:\n";

    for (int i = 0; i < 0x30; i += 0x8) {
        uintptr_t value = memory->read<uintptr_t>(bytecode_string_ptr + i);
        debug += "+" + std::to_string(i) + ": 0x" + std::to_string(value) + "\n";
    }

    return debug;
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

    ImGui::Text("Address: 0x%llX", selected_instance);
    std::string name = core.get_instance_name(selected_instance);
    ImGui::Text("Name: %s", name.c_str());

    std::string class_name = core.get_instance_classname(selected_instance);
    ImGui::Text("ClassName: %s", class_name.c_str());

    ImGui::Separator();

    if (class_name == "LocalScript" || class_name == "ModuleScript") {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        if (ImGui::Button("View Script")) {
            script_viewer_content = dump_script_to_string(selected_instance, class_name);
            script_viewer_title = name + " [" + class_name + "]";
            show_script_viewer = true;
        }
        ImGui::PopStyleColor();
        ImGui::Separator();
    }

    bool anchored = memory->read<bool>(selected_instance + offsets::Anchored);
    ImGui::Checkbox("Anchored", &anchored);

    vector position = memory->read<vector>(selected_instance + offsets::Position);
    ImGui::Text("Position: %.3f, %.3f, %.3f", position.x, position.y, position.z);

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

            uintptr_t local_player_character_model_temp = core.find_first_child(core.find_first_child_class(g_main::datamodel, "Workspace"), core.get_instance_name(g_main::localplayer));
            if (!local_player_character_model_temp) {
                util.m_print("Teleport: Could not teleport: Local player character model not found.");
                ImGui::EndChild();
                return;
            }

            uintptr_t hrp_temp = core.find_first_child(local_player_character_model_temp, "HumanoidRootPart");
            if (!hrp_temp) {
                util.m_print("Teleport: Could not teleport: Local HumanoidRootPart not found.");
                ImGui::EndChild();
                return;
            }

            uintptr_t p_local_root = memory->read<uintptr_t>(hrp_temp + offsets::Primitive);
            if (!p_local_root) {
                util.m_print("Teleport: Could not teleport: Local HumanoidRootPart Primitive not found.");
                ImGui::EndChild();
                return;
            }

            vector w_target_pos = { 0, 0, 0 };
            bool position_found = false;

            std::string target_class_name = core.get_instance_classname(selected_instance);

            if (target_class_name.find("Model") != std::string::npos) {
                uintptr_t primary_part = 0;

                uintptr_t hrp_in_model = core.find_first_child(selected_instance, "HumanoidRootPart");
                if (hrp_in_model) {
                    primary_part = hrp_in_model;
                }
                else {
                    std::vector<uintptr_t> children = core.children(selected_instance);
                    for (uintptr_t child : children) {
                        std::string child_class_name = core.get_instance_classname(child);
                        if (child_class_name.find("Part") != std::string::npos) {
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
                uintptr_t target_primitive = memory->read<uintptr_t>(selected_instance + offsets::Primitive);
                if (target_primitive) {
                    w_target_pos = memory->read<vector>(target_primitive + offsets::Position);
                    position_found = true;
                }
            }

            if (!position_found) {
                util.m_print("Teleport: Could not get target position from selected instance.");
                ImGui::EndChild();
                return;
            }

            w_target_pos.y += vars::misc::teleport_offset_y;
            w_target_pos.z += vars::misc::teleport_offset_z;

            if (!std::isfinite(w_target_pos.x) || !std::isfinite(w_target_pos.y) || !std::isfinite(w_target_pos.z)) {
                util.m_print("Teleport: Aborting due to invalid position");
                ImGui::EndChild();
                return;
            }

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

        // Quick navigation buttons
        if (ImGui::Button("Workspace")) {
            selected_instance = core.find_first_child_class(g_main::datamodel, "Workspace");
        }
        ImGui::SameLine();
        if (ImGui::Button("ReplicatedStorage")) {
            selected_instance = core.find_first_child_class(g_main::datamodel, "ReplicatedStorage");
        }
        ImGui::SameLine();
        if (ImGui::Button("Players")) {
            selected_instance = core.find_first_child_class(g_main::datamodel, "Players");
        }
        ImGui::SameLine();
        if (ImGui::Button("ReplicatedFirst")) {
            selected_instance = core.find_first_child_class(g_main::datamodel, "ReplicatedFirst");
        }
        ImGui::SameLine();
        if (ImGui::Button("StarterGui")) {
            selected_instance = core.find_first_child_class(g_main::datamodel, "StarterGui");
        }

        ImGui::Separator();

        // Show all direct DataModel children
        if (ImGui::CollapsingHeader("DataModel Direct Children")) {
            if (g_main::datamodel) {
                std::vector<uintptr_t> dm_children = core.children(g_main::datamodel);
                for (uintptr_t child : dm_children) {
                    std::string name = core.get_instance_name(child);
                    std::string class_name = core.get_instance_classname(child);
                    if (ImGui::Selectable((name + " [" + class_name + "]").c_str())) {
                        selected_instance = child;
                    }
                }
            }
        }

        ImGui::Separator();

        ImGui::Columns(2);

        ImGui::BeginChild("Tree");
        if (g_main::datamodel) {
            draw_instance_node(g_main::datamodel);
        }
        ImGui::EndChild();

        ImGui::NextColumn();
        draw_properties();

        ImGui::Columns(1);
    }
    ImGui::End();

    if (show_script_viewer) {
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        if (ImGui::Begin(script_viewer_title.c_str(), &show_script_viewer)) {
            ImGui::TextWrapped("%s", script_viewer_content.c_str());
        }
        ImGui::End();
    }
}


c_workspace_viewer workspace_viewer;
