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

    uintptr_t local_player_obj = g_main::localplayer;
    if (!local_player_obj) return;

    uintptr_t player_mouse = memory->read<uintptr_t>(local_player_obj + offsets::PlayerMouse);
    if (!player_mouse) return;

    uintptr_t camera_obj = memory->read<uintptr_t>(player_mouse + offsets::Camera);
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
    ImGui::BeginChild("Properties", ImVec2(0, 0), ImGuiChildFlags_Borders);
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
            misc.teleport_to(selected_instance);
        }
    }
    else if (class_name.find("Part") != std::string::npos || class_name.find("Model") != std::string::npos) {
        ImGui::Separator();
        if (ImGui::Button("Teleport to")) {
            uintptr_t local_player_character_model_temp = core.find_first_child(core.find_first_child_class(g_main::datamodel, "Workspace"), core.get_instance_name(g_main::localplayer));
            if (!local_player_character_model_temp) {
                ImGui::EndChild();
                return;
            }

            uintptr_t hrp_temp = core.find_first_child(local_player_character_model_temp, "HumanoidRootPart");
            if (!hrp_temp) {
                ImGui::EndChild();
                return;
            }

            uintptr_t p_local_root = memory->read<uintptr_t>(hrp_temp + offsets::Primitive);
            if (!p_local_root) {
                ImGui::EndChild();
                return;
            }

            vector w_target_pos = { 0, 0, 0 };
            bool position_found = false;

            std::string target_class_name = core.get_instance_classname(selected_instance);

            if (target_class_name.find("Model") != std::string::npos) {
                uintptr_t primary_part = core.find_first_child(selected_instance, "HumanoidRootPart");
                if (!primary_part) {
                    std::vector<uintptr_t> children = core.children(selected_instance);
                    for (uintptr_t child : children) {
                        if (core.get_instance_classname(child).find("Part") != std::string::npos) {
                            primary_part = child;
                            break;
                        }
                    }
                }

                if (primary_part) {
                    uintptr_t prim = memory->read<uintptr_t>(primary_part + offsets::Primitive);
                    if (prim) {
                        w_target_pos = memory->read<vector>(prim + offsets::Position);
                        position_found = true;
                    }
                }
            }
            else {
                uintptr_t prim = memory->read<uintptr_t>(selected_instance + offsets::Primitive);
                if (prim) {
                    w_target_pos = memory->read<vector>(prim + offsets::Position);
                    position_found = true;
                }
            }

            if (position_found) {
                w_target_pos.y += vars::misc::teleport_offset_y;
                misc.teleport_to_position(p_local_root, w_target_pos);
            }
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

void c_workspace_viewer::search_recursive(uintptr_t instance, const std::string& query, std::vector<uintptr_t>& results)
{
    if (!instance) return;
    if (results.size() >= 100) return;

    std::string name = core.get_instance_name(instance);
    std::string class_name = core.get_instance_classname(instance);

    std::string name_lower = name;
    std::string class_lower = class_name;
    std::string query_lower = query;

    for (char& c : name_lower) c = tolower(c);
    for (char& c : class_lower) c = tolower(c);
    for (char& c : query_lower) c = tolower(c);

    if (name_lower.find(query_lower) != std::string::npos ||
        class_lower.find(query_lower) != std::string::npos)
    {
        results.push_back(instance);
    }

    std::vector<uintptr_t> children = core.children(instance);
    for (uintptr_t child : children)
    {
        search_recursive(child, query, results);
    }
}

void c_workspace_viewer::perform_search(uintptr_t root, const std::string& query)
{
    search_results.clear();

    if (query.empty() || !root) {
        search_performed = false;
        return;
    }

    search_recursive(root, query, search_results);
    search_performed = true;
}

void c_workspace_viewer::run() {
    ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Workspace Viewer", &vars::misc::show_workspace_viewer)) {

        // Navigation buttons
        if (ImGui::Button("Workspace")) {
            selected_instance = core.find_first_child_class(g_main::datamodel, "Workspace");
            search_performed = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("ReplicatedStorage")) {
            selected_instance = core.find_first_child_class(g_main::datamodel, "ReplicatedStorage");
            search_performed = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Players")) {
            selected_instance = core.find_first_child_class(g_main::datamodel, "Players");
            search_performed = false;
        }

        ImGui::Separator();

        // Search bar
        ImGui::Text("Search:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        bool enter_pressed = ImGui::InputText("##search", search_buffer, sizeof(search_buffer), ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::SameLine();
        if (ImGui::Button("Go") || enter_pressed) {
            if (g_main::datamodel) {
                perform_search(g_main::datamodel, search_buffer);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear")) {
            search_buffer[0] = '\0';
            search_results.clear();
            search_performed = false;
        }
        if (search_performed) {
            ImGui::SameLine();
            ImGui::Text("(%d)", (int)search_results.size());
        }

        ImGui::Separator();

        // Left panel
        ImGui::BeginChild("LeftPanel", ImVec2(300, 0), ImGuiChildFlags_Borders);

        if (search_performed && !search_results.empty()) {
            ImGui::Text("Results:");
            for (uintptr_t result : search_results) {
                std::string name = core.get_instance_name(result);
                std::string cname = core.get_instance_classname(result);
                if (ImGui::Selectable((name + " [" + cname + "]").c_str(), selected_instance == result)) {
                    selected_instance = result;
                }
            }
        }
        else if (search_performed) {
            ImGui::Text("No results");
        }
        else {
            if (g_main::datamodel) {
                draw_instance_node(g_main::datamodel);
            }
        }

        ImGui::EndChild();

        ImGui::SameLine();
        draw_properties();
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