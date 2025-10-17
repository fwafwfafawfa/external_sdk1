#pragma once
#include "../../main.hpp"

class c_workspace_viewer {
public:
    void run();
    void draw_selected_instance_highlight(); // New method

private:
    void draw_instance_node(uintptr_t instance);
    void draw_properties();

    uintptr_t selected_instance = 0;
};

extern c_workspace_viewer workspace_viewer;