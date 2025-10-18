#include "config.hpp"
#include <iostream>
#include <sstream>
#include <limits>

// Helper to convert ImColor to a string (RGBA)
std::string c_config::color_to_string( const ImColor& color )
{
    std::stringstream ss;
    ss << static_cast<int>(color.Value.x * 255.0f) << ","
       << static_cast<int>(color.Value.y * 255.0f) << ","
       << static_cast<int>(color.Value.z * 255.0f) << ","
       << static_cast<int>(color.Value.w * 255.0f);
    return ss.str();
}

// Helper to convert string to ImColor (RGBA)
ImColor c_config::string_to_color( const std::string& str )
{
    std::stringstream ss(str);
    std::string segment;
    std::vector<int> rgba;

    while(std::getline(ss, segment, ','))
    {
        rgba.push_back(std::stoi(segment));
    }

    if (rgba.size() == 4)
    {
        return ImColor(rgba[0], rgba[1], rgba[2], rgba[3]);
    }
    return ImColor(255, 255, 255, 255); // Default to white if parsing fails
}

// Helper to get a value from a map
template<typename T>
T c_config::get_value( const std::map<std::string, std::string>& data, const std::string& key, T default_value )
{
    auto it = data.find(key);
    if (it != data.end())
    {
        try
        {
            if constexpr (std::is_same_v<T, bool>)
            {
                return it->second == "true";
            }
            else if constexpr (std::is_same_v<T, float>)
            {
                return std::stof(it->second);
            }
            else if constexpr (std::is_same_v<T, int>)
            {
                return std::stoi(it->second);
            }
            else if constexpr (std::is_same_v<T, ImColor>)
            {
                return string_to_color(it->second);
            }
            else
            {
                return default_value; // Unsupported type
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing config value for key " << key << ": " << e.what() << std::endl;
        }
    }
    return default_value;
}

void c_config::save( const std::string& filename )
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        // ESP
        file << "esp_toggled=" << (vars::esp::toggled ? "true" : "false") << std::endl;
        file << "esp_show_health=" << (vars::esp::show_health ? "true" : "false") << std::endl;
        file << "esp_show_distance=" << (vars::esp::show_distance ? "true" : "false") << std::endl;
        file << "esp_show_skeleton=" << (vars::esp::show_skeleton ? "true" : "false") << std::endl;
        file << "esp_box_color=" << color_to_string(vars::esp::esp_box_color) << std::endl;
        file << "esp_name_color=" << color_to_string(vars::esp::esp_name_color) << std::endl;
        file << "esp_distance_color=" << color_to_string(vars::esp::esp_distance_color) << std::endl;
        file << "esp_skeleton_color=" << color_to_string(vars::esp::esp_skeleton_color) << std::endl;

        // Aimbot
        file << "aimbot_toggled=" << (vars::aimbot::toggled ? "true" : "false") << std::endl;
        file << "aimbot_speed=" << vars::aimbot::speed << std::endl;
        file << "aimbot_fov=" << vars::aimbot::fov << std::endl;
        file << "aimbot_deadzone=" << vars::aimbot::deadzone << std::endl;
        file << "aimbot_use_set_cursor_pos=" << (vars::aimbot::use_set_cursor_pos ? "true" : "false") << std::endl;
        file << "aimbot_smoothing_factor=" << vars::aimbot::smoothing_factor << std::endl;
        file << "aimbot_activation_key=" << vars::aimbot::activation_key << std::endl;
        file << "aimbot_show_fov_circle=" << (vars::aimbot::show_fov_circle ? "true" : "false") << std::endl;
        file << "aimbot_prediction=" << (vars::aimbot::prediction ? "true" : "false") << std::endl;

        // Speed Hack
        file << "speed_hack_toggled=" << (vars::speed_hack::toggled ? "true" : "false") << std::endl;
        file << "speed_hack_value=" << vars::speed_hack::value << std::endl;

        // Jump Power
        file << "jump_power_toggled=" << (vars::jump_power::toggled ? "true" : "false") << std::endl;
        file << "jump_power_value=" << vars::jump_power::value << std::endl;
        file << "jump_power_default_value=" << vars::jump_power::default_value << std::endl;

        // Misc
        file << "misc_show_workspace_viewer=" << (vars::misc::show_workspace_viewer ? "true" : "false") << std::endl;
        file << "misc_teleport_offset_y=" << vars::misc::teleport_offset_y << std::endl;
        file << "misc_teleport_offset_z=" << vars::misc::teleport_offset_z << std::endl;

        file.close();
        std::cout << "Config saved to " << filename << std::endl;
    }
    else
    {
        std::cerr << "Error: Could not open file for saving config: " << filename << std::endl;
    }
}

void c_config::load( const std::string& filename )
{
    std::ifstream file(filename);
    if (file.is_open())
    {
        std::map<std::string, std::string> data;
        std::string line;
        while (std::getline(file, line))
        {
            size_t delimiterPos = line.find('=');
            if (delimiterPos != std::string::npos)
            {
                std::string key = line.substr(0, delimiterPos);
                std::string value = line.substr(delimiterPos + 1);
                data[key] = value;
            }
        }
        file.close();

        // ESP
        vars::esp::toggled = get_value(data, "esp_toggled", vars::esp::toggled);
        vars::esp::show_health = get_value(data, "esp_show_health", vars::esp::show_health);
        vars::esp::show_distance = get_value(data, "esp_show_distance", vars::esp::show_distance);
        vars::esp::show_skeleton = get_value(data, "esp_show_skeleton", vars::esp::show_skeleton);
        vars::esp::esp_box_color = get_value(data, "esp_box_color", vars::esp::esp_box_color);
        vars::esp::esp_name_color = get_value(data, "esp_name_color", vars::esp::esp_name_color);
        vars::esp::esp_distance_color = get_value(data, "esp_distance_color", vars::esp::esp_distance_color);
        vars::esp::esp_skeleton_color = get_value(data, "esp_skeleton_color", vars::esp::esp_skeleton_color);

        // Aimbot
        vars::aimbot::toggled = get_value(data, "aimbot_toggled", vars::aimbot::toggled);
        vars::aimbot::speed = get_value(data, "aimbot_speed", vars::aimbot::speed);
        vars::aimbot::fov = get_value(data, "aimbot_fov", vars::aimbot::fov);
        vars::aimbot::deadzone = get_value(data, "aimbot_deadzone", vars::aimbot::deadzone);
        vars::aimbot::use_set_cursor_pos = get_value(data, "aimbot_use_set_cursor_pos", vars::aimbot::use_set_cursor_pos);
        vars::aimbot::smoothing_factor = get_value(data, "aimbot_smoothing_factor", vars::aimbot::smoothing_factor);
        vars::aimbot::activation_key = get_value(data, "aimbot_activation_key", vars::aimbot::activation_key);
        vars::aimbot::show_fov_circle = get_value(data, "aimbot_show_fov_circle", vars::aimbot::show_fov_circle);
        vars::aimbot::prediction = get_value(data, "aimbot_prediction", vars::aimbot::prediction);

        // Speed Hack
        vars::speed_hack::toggled = get_value(data, "speed_hack_toggled", vars::speed_hack::toggled);
        vars::speed_hack::value = get_value(data, "speed_hack_value", vars::speed_hack::value);

        // Jump Power
        vars::jump_power::toggled = get_value(data, "jump_power_toggled", vars::jump_power::toggled);
        vars::jump_power::value = get_value(data, "jump_power_value", vars::jump_power::value);
        vars::jump_power::default_value = get_value(data, "jump_power_default_value", vars::jump_power::default_value);

        // Misc
        vars::misc::show_workspace_viewer = get_value(data, "misc_show_workspace_viewer", vars::misc::show_workspace_viewer);
        vars::misc::teleport_offset_y = get_value(data, "misc_teleport_offset_y", vars::misc::teleport_offset_y);
        vars::misc::teleport_offset_z = get_value(data, "misc_teleport_offset_z", vars::misc::teleport_offset_z);

        std::cout << "Config loaded from " << filename << std::endl;
    }
    else
    {
        std::cerr << "Error: Could not open file for loading config: " << filename << std::endl;
    }
}
