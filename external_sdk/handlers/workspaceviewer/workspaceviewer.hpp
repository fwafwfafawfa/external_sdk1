#pragma once
#include "../../main.hpp"
#include "../../game/core.hpp"
#include "../../addons/kernel/memory.hpp"
#include "../../game/offsets/offsets.hpp"
#include "../../handlers/overlay/draw.hpp"
#include "../../handlers/vars.hpp"
#include <vector>
#include <string>

class c_workspace_viewer {
public:
    void run();
    void draw_selected_instance_highlight();

private:
    uintptr_t selected_instance = 0;
    bool show_script_viewer = false;
    std::string script_viewer_content;
    std::string script_viewer_title;

    // Search functionality
    char search_buffer[256] = "";
    std::vector<uintptr_t> search_results;
    bool search_performed = false;

    void draw_properties();
    void draw_instance_node(uintptr_t instance);
    std::string dump_script_to_string(uintptr_t script_instance, const std::string& script_type);
    void perform_search(uintptr_t root, const std::string& query);
    void search_recursive(uintptr_t instance, const std::string& query, std::vector<uintptr_t>& results);
};

inline c_workspace_viewer workspace_viewer;