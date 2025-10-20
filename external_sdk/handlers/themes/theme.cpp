#include "theme.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

// Helper to convert ImVec4 to a string (RGBA)
std::string c_theme::color_to_string(const ImVec4& color)
{
    std::stringstream ss;
    ss << static_cast<int>(color.x * 255.0f) << ","
        << static_cast<int>(color.y * 255.0f) << ","
        << static_cast<int>(color.z * 255.0f) << ","
        << static_cast<int>(color.w * 255.0f);
    return ss.str();
}

// Helper to convert string to ImVec4 (RGBA)
ImVec4 c_theme::string_to_color(const std::string& str)
{
    std::vector<int> rgba;
    std::string temp = str;
    size_t pos = 0;
    std::string token;
    while ((pos = temp.find(',')) != std::string::npos) {
        token = temp.substr(0, pos);
        try {
            rgba.push_back(std::stoi(token));
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing color segment: " << token << " - " << e.what() << std::endl;
            return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        temp.erase(0, pos + 1);
    }
    try {
        rgba.push_back(std::stoi(temp));
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing color segment: " << temp << " - " << e.what() << std::endl;
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }


    if (rgba.size() == 4)
    {
        return ImVec4(rgba[0] / 255.0f, rgba[1] / 255.0f, rgba[2] / 255.0f, rgba[3] / 255.0f);
    }
    return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
}

void c_theme::save(const std::string& filename)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        ImGuiStyle& style = ImGui::GetStyle();

        file << "[colors]" << std::endl;
        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
            file << i << "=" << color_to_string(style.Colors[i]) << std::endl;
        }

        file.close();
        std::cout << "Theme saved to " << filename << std::endl;
    }
    else
    {
        std::cerr << "Error: Could not open file for saving theme: " << filename << std::endl;
    }
}

void c_theme::load(const std::string& filename)
{
    std::ifstream file(filename);
    if (file.is_open())
    {
        std::map<int, std::string> data;
        std::string line;
        while (std::getline(file, line))
        {
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            if (line.empty() || line[0] == ';' || line[0] == '#' || line[0] == '[')
                continue;

            size_t delimiterPos = line.find('=');
            if (delimiterPos != std::string::npos)
            {
                try
                {
                    int key = std::stoi(line.substr(0, delimiterPos));
                    std::string value = line.substr(delimiterPos + 1);
                    data[key] = value;
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Error parsing theme line: " << line << " - " << e.what() << std::endl;
                }
            }
        }
        file.close();

        ImGuiStyle& style = ImGui::GetStyle();

        for (auto const& [key, val] : data)
        {
            if (key >= 0 && key < ImGuiCol_COUNT)
            {
                style.Colors[key] = string_to_color(val);
            }
        }

        std::cout << "Theme loaded from " << filename << std::endl;
    }
    else
    {
        std::cerr << "Error: Could not open file for loading theme: " << filename << std::endl;
    }
}
