// workspaceviewer.hpp
#pragma once
#include "../../main.hpp"
#include <fstream>

class c_workspace_viewer {
public:
    void run();
    void draw_selected_instance_highlight();

private:
    void draw_instance_node(uintptr_t instance);
    void draw_properties();
    std::string dump_script_to_string(uintptr_t script_instance, const std::string& script_type);

    uintptr_t selected_instance = 0;
    std::string script_viewer_content = "";
    bool show_script_viewer = false;
    std::string script_viewer_title = "";
};

extern c_workspace_viewer workspace_viewer;
