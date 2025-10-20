#pragma once

#include "../../main.hpp"
#include "addons/imgui/imgui.h"
#include <string>
#include <vector>
#include <map>

class c_theme
{
public:
    void save(const std::string& filename);
    void load(const std::string& filename);

private:
    std::string color_to_string(const ImVec4& color);
    ImVec4 string_to_color(const std::string& str);
};

inline c_theme theme;
