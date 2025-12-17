#include "config.hpp"
#include <iostream>
#include <sstream>
#include <limits>

// Helper to convert ImColor to a string (RGBA)
std::string c_config::color_to_string(const ImColor& color)
{
    std::stringstream ss;
    ss << static_cast<int>(color.Value.x * 255.0f) << ","
        << static_cast<int>(color.Value.y * 255.0f) << ","
        << static_cast<int>(color.Value.z * 255.0f) << ","
        << static_cast<int>(color.Value.w * 255.0f);
    return ss.str();
}

// Helper to convert string to ImColor (RGBA)
ImColor c_config::string_to_color(const std::string& str)
{
    std::stringstream ss(str);
    std::string segment;
    std::vector<int> rgba;

    while (std::getline(ss, segment, ','))
    {
        try {
            rgba.push_back(std::stoi(segment));
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing color segment: " << segment << " - " << e.what() << std::endl;
            return ImColor(255, 255, 255, 255); // Default to white if parsing fails
        }
    }

    if (rgba.size() == 4)
    {
        return ImColor(rgba[0], rgba[1], rgba[2], rgba[3]);
    }
    return ImColor(255, 255, 255, 255); // Default to white if parsing fails
}

// Helper to get a value from a map
template<typename T>
T c_config::get_value(const std::map<std::string, std::string>& data, const std::string& key, T default_value)
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

void c_config::save(const std::string& filename)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        // ESP
        file << "[esp]" << std::endl;
        file << "esp_toggled=" << (vars::esp::toggled ? "true" : "false") << std::endl;
        file << "esp_show_health=" << (vars::esp::show_health ? "true" : "false") << std::endl;
        file << "esp_show_distance=" << (vars::esp::show_distance ? "true" : "false") << std::endl;
        file << "esp_show_skeleton=" << (vars::esp::show_skeleton ? "true" : "false") << std::endl;
        file << "esp_show_tracers=" << (vars::esp::show_tracers ? "true" : "false") << std::endl;
        file << "esp_show_box=" << (vars::esp::show_box ? "true" : "false") << std::endl;
        file << "esp_hide_dead=" << (vars::esp::hide_dead ? "true" : "false") << std::endl;
        file << "esp_hide_teammates=" << (vars::esp::hide_teammates ? "true" : "false") << std::endl;
        file << "esp_box_color=" << color_to_string(vars::esp::esp_box_color) << std::endl;
        file << "esp_name_color=" << color_to_string(vars::esp::esp_name_color) << std::endl;
        file << "esp_distance_color=" << color_to_string(vars::esp::esp_distance_color) << std::endl;
        file << "esp_skeleton_color=" << color_to_string(vars::esp::esp_skeleton_color) << std::endl;
        file << "esp_tracer_color=" << color_to_string(vars::esp::esp_tracer_color) << std::endl;

        // Aimbot
        file << "[aimbot]" << std::endl;
        file << "aimbot_toggled=" << (vars::aimbot::toggled ? "true" : "false") << std::endl;
        file << "aimbot_speed=" << vars::aimbot::speed << std::endl;
        file << "aimbot_fov=" << vars::aimbot::fov << std::endl;
        file << "aimbot_deadzone=" << vars::aimbot::deadzone << std::endl;
        file << "aimbot_use_set_cursor_pos=" << (vars::aimbot::use_set_cursor_pos ? "true" : "false") << std::endl;
        file << "aimbot_smoothing_factor=" << vars::aimbot::smoothing_factor << std::endl;
        file << "aimbot_activation_key=" << vars::aimbot::activation_key << std::endl;
        file << "aimbot_show_fov_circle=" << (vars::aimbot::show_fov_circle ? "true" : "false") << std::endl;
        file << "aimbot_prediction=" << (vars::aimbot::prediction ? "true" : "false") << std::endl;
        file << "aimbot_hitbox=" << vars::aimbot::aimbot_hitbox << std::endl;
        file << "aimbot_ignore_teammates=" << (vars::aimbot::ignore_teammates ? "true" : "false") << std::endl;
        file << "aimbot_prediction_x=" << vars::aimbot::prediction_x << std::endl;
        file << "aimbot_prediction_y=" << vars::aimbot::prediction_y << std::endl;
        file << "aimbot_prediction_ignore_y=" << (vars::aimbot::prediction_ignore_y ? "true" : "false") << std::endl;
        file << "aimbot_sticky_aim=" << (vars::aimbot::sticky_aim ? "true" : "false") << std::endl;
        file << "aimbot_air_part_enabled=" << (vars::aimbot::air_part_enabled ? "true" : "false") << std::endl;
        file << "aimbot_air_part_hitbox=" << vars::aimbot::air_part_hitbox << std::endl;
        file << "aimbot_anti_flick=" << (vars::aimbot::anti_flick ? "true" : "false") << std::endl;
        file << "aimbot_anti_flick_distance=" << vars::aimbot::anti_flick_distance << std::endl;
        file << "aimbot_smoothing_style=" << vars::aimbot::smoothing_style << std::endl;
        file << "aimbot_shake=" << (vars::aimbot::shake ? "true" : "false") << std::endl;
        file << "aimbot_shake_x=" << vars::aimbot::shake_x << std::endl;
        file << "aimbot_shake_y=" << vars::aimbot::shake_y << std::endl;
        file << "aimbot_unlock_on_death=" << (vars::aimbot::unlock_on_death ? "true" : "false") << std::endl;


        // Triggerbot
        file << "[triggerbot]" << std::endl;
        file << "triggerbot_toggled=" << (vars::triggerbot::toggled ? "true" : "false") << std::endl;
        file << "triggerbot_fov=" << vars::triggerbot::fov << std::endl;
        file << "triggerbot_delay=" << vars::triggerbot::delay << std::endl;
        file << "triggerbot_hold_time=" << vars::triggerbot::hold_time << std::endl;
        file << "triggerbot_activation_key=" << vars::triggerbot::activation_key << std::endl;


        // Speed Hack
        file << "[speed_hack]" << std::endl;
        file << "speed_hack_toggled=" << (vars::speed_hack::toggled ? "true" : "false") << std::endl;
        file << "speed_hack_value=" << vars::speed_hack::value << std::endl;

        // Infinite Jump
        file << "[infinite_jump]" << std::endl;
        file << "infinite_jump_toggled=" << (vars::infinite_jump::toggled ? "true" : "false") << std::endl;
        file << "infinite_jump_jump_power_value=" << vars::infinite_jump::jump_power_value << std::endl;
        file << "jump_power_toggled=" << (vars::jump_power::toggled ? "true" : "false") << std::endl;
        file << "jump_power_value=" << vars::jump_power::value << std::endl;
        file << "jump_power_default_value=" << vars::jump_power::default_value << std::endl;

        // Fly
        file << "[fly]" << std::endl;
        file << "fly_toggled=" << (vars::fly::toggled ? "true" : "false") << std::endl;
        file << "fly_speed=" << vars::fly::speed << std::endl;
        file << "fly_toggle_key=" << vars::fly::fly_toggle_key << std::endl;
        file << "fly_mode=" << vars::fly::fly_mode << std::endl;

        // Misc
        file << "[misc]" << std::endl;
        file << "misc_show_workspace_viewer=" << (vars::misc::show_workspace_viewer ? "true" : "false") << std::endl;
        file << "misc_teleport_offset_y=" << vars::misc::teleport_offset_y << std::endl;
        file << "misc_teleport_offset_z=" << vars::misc::teleport_offset_z << std::endl;

        // Lag Switch
        file << "[lag_switch]" << std::endl;
        file << "lag_switch_toggled=" << (vars::lag_switch::toggled ? "true" : "false") << std::endl;
        file << "lag_switch_activation_key=" << vars::lag_switch::activation_key << std::endl;
        file << "lag_switch_lag_duration=" << vars::lag_switch::lag_duration << std::endl;
        file << "lag_switch_lag_interval=" << vars::lag_switch::lag_interval << std::endl;
        file << "lag_switch_auto_lag=" << (vars::lag_switch::auto_lag ? "true" : "false") << std::endl;
        file << "lag_switch_manual_lag=" << (vars::lag_switch::manual_lag ? "true" : "false") << std::endl;

		// Set FOV
        file << "[set_fov]" << std::endl;
        file << "set_fov=" << vars::set_fov::set_fov << std::endl;
        file << "toggled=" << (vars::set_fov::toggled ? "true" : "false") << std::endl;
        file << "unlock_zoom=" << (vars::set_fov::unlock_zoom ? "true" : "false") << std::endl;

        // Expander
        file << "[combat]" << std::endl;
        file << "hitbox_expander=" << (vars::combat::hitbox_expander ? "true" : "false") << std::endl;
        file << "hitbox_size_x=" << vars::combat::hitbox_size_x << std::endl;
        file << "hitbox_size_y=" << vars::combat::hitbox_size_y << std::endl;
        file << "hitbox_size_z=" << vars::combat::hitbox_size_z << std::endl;
        file << "hitbox_multiplier=" << vars::combat::hitbox_multiplier << std::endl;
        file << "hitbox_visible=" << (vars::combat::hitbox_visible ? "true" : "false") << std::endl;
        file << "hitbox_can_collide=" << (vars::combat::hitbox_can_collide ? "true" : "false") << std::endl;
        file << "hitbox_skip_teammates=" << (vars::combat::hitbox_skip_teammates ? "true" : "false") << std::endl;


        file.close();
        std::cout << "Config saved to " << filename << std::endl;
    }
    else
    {
        std::cerr << "Error: Could not open file for saving config: " << filename << std::endl;
    }
}

void c_config::load(const std::string& filename)
{
    std::ifstream file(filename);
    if (file.is_open())
    {
        std::map<std::string, std::string> data;
        std::string line;
        while (std::getline(file, line))
        {
            // Remove leading/trailing whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            if (line.empty() || line[0] == ';' || line[0] == '#') // Skip empty lines and comments
                continue;

            // Handle sections (e.g., [section_name])
            if (line[0] == '[' && line.back() == ']')
            {
                continue; // Sections are only for file organization, not used in data
            }

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
        vars::esp::show_tracers = get_value(data, "esp_show_tracers", vars::esp::show_tracers);
        vars::esp::show_box = get_value(data, "esp_show_box", vars::esp::show_box);
        vars::esp::hide_dead = get_value(data, "esp_hide_dead", vars::esp::hide_dead);
        vars::esp::hide_teammates = get_value(data, "esp_hide_teammates", vars::esp::hide_teammates);
        vars::esp::esp_box_color = get_value(data, "esp_box_color", vars::esp::esp_box_color);
        vars::esp::esp_name_color = get_value(data, "esp_name_color", vars::esp::esp_name_color);
        vars::esp::esp_distance_color = get_value(data, "esp_distance_color", vars::esp::esp_distance_color);
        vars::esp::esp_skeleton_color = get_value(data, "esp_skeleton_color", vars::esp::esp_skeleton_color);
        vars::esp::esp_tracer_color = get_value(data, "esp_tracer_color", vars::esp::esp_tracer_color);

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
        vars::aimbot::air_part_enabled = get_value(data, "aimbot_air_part_enabled", vars::aimbot::air_part_enabled);
        vars::aimbot::aimbot_hitbox = get_value(data, "aimbot_hitbox", vars::aimbot::aimbot_hitbox);
        vars::aimbot::ignore_teammates = get_value(data, "aimbot_ignore_teammates", vars::aimbot::ignore_teammates);
        vars::aimbot::prediction_x = get_value(data, "aimbot_prediction_x", vars::aimbot::prediction_x);
        vars::aimbot::prediction_y = get_value(data, "aimbot_prediction_y", vars::aimbot::prediction_y);
        vars::aimbot::prediction_ignore_y = get_value(data, "aimbot_prediction_ignore_y", vars::aimbot::prediction_ignore_y);
        vars::aimbot::sticky_aim = get_value(data, "aimbot_sticky_aim", vars::aimbot::sticky_aim);
        vars::aimbot::air_part_enabled = get_value(data, "aimbot_air_part_enabled", vars::aimbot::air_part_enabled);
        vars::aimbot::air_part_hitbox = get_value(data, "aimbot_air_part_hitbox", vars::aimbot::air_part_hitbox);
        vars::aimbot::anti_flick = get_value(data, "aimbot_anti_flick", vars::aimbot::anti_flick);
        vars::aimbot::anti_flick_distance = get_value(data, "aimbot_anti_flick_distance", vars::aimbot::anti_flick_distance);
        vars::aimbot::smoothing_style = get_value(data, "aimbot_smoothing_style", vars::aimbot::smoothing_style);
        vars::aimbot::shake = get_value(data, "aimbot_shake", vars::aimbot::shake);
        vars::aimbot::shake_x = get_value(data, "aimbot_shake_x", vars::aimbot::shake_x);
        vars::aimbot::shake_y = get_value(data, "aimbot_shake_y", vars::aimbot::shake_y);
        vars::aimbot::unlock_on_death = get_value(data, "aimbot_unlock_on_death", vars::aimbot::unlock_on_death);




        // Triggerbot
        vars::triggerbot::toggled = get_value(data, "triggerbot_toggled", vars::triggerbot::toggled);
        vars::triggerbot::fov = get_value(data, "triggerbot_fov", vars::triggerbot::fov);
        vars::triggerbot::delay = get_value(data, "triggerbot_delay", vars::triggerbot::delay);
        vars::triggerbot::hold_time = get_value(data, "triggerbot_hold_time", vars::triggerbot::hold_time);
        vars::triggerbot::activation_key = get_value(data, "triggerbot_activation_key", vars::triggerbot::activation_key);


        // Speed Hack
        vars::speed_hack::toggled = get_value(data, "speed_hack_toggled", vars::speed_hack::toggled);
        vars::speed_hack::value = get_value(data, "speed_hack_value", vars::speed_hack::value);

        // Infinite Jump
        vars::infinite_jump::toggled = get_value(data, "infinite_jump_toggled", vars::infinite_jump::toggled);
        vars::infinite_jump::jump_power_value = get_value(data, "infinite_jump_jump_power_value", vars::infinite_jump::jump_power_value);
        vars::jump_power::value = get_value(data, "jump_power_value", vars::jump_power::value);
        vars::jump_power::default_value = get_value(data, "jump_power_default_value", vars::jump_power::default_value);

        // Fly
        vars::fly::toggled = get_value(data, "fly_toggled", vars::fly::toggled);
        vars::fly::speed = get_value(data, "fly_speed", vars::fly::speed);
        vars::fly::fly_toggle_key = get_value(data, "fly_toggle_key", vars::fly::fly_toggle_key);
        vars::fly::fly_mode = get_value(data, "fly_mode", vars::fly::fly_mode);

        // Misc
        vars::misc::show_workspace_viewer = get_value(data, "misc_show_workspace_viewer", vars::misc::show_workspace_viewer);
        vars::misc::teleport_offset_y = get_value(data, "misc_teleport_offset_y", vars::misc::teleport_offset_y);
        vars::misc::teleport_offset_z = get_value(data, "misc_teleport_offset_z", vars::misc::teleport_offset_z);

        // Lag Switch
        vars::lag_switch::toggled = get_value(data, "lag_switch_toggled", vars::lag_switch::toggled);
        vars::lag_switch::activation_key = get_value(data, "lag_switch_activation_key", vars::lag_switch::activation_key);
        vars::lag_switch::lag_duration = get_value(data, "lag_switch_lag_duration", vars::lag_switch::lag_duration);
        vars::lag_switch::lag_interval = get_value(data, "lag_switch_lag_interval", vars::lag_switch::lag_interval);
        vars::lag_switch::auto_lag = get_value(data, "lag_switch_auto_lag", vars::lag_switch::auto_lag);
        vars::lag_switch::manual_lag = get_value(data, "lag_switch_manual_lag", vars::lag_switch::manual_lag);

        // FOV
        vars::set_fov::set_fov = get_value(data, "set_fov", vars::set_fov::set_fov);
        vars::set_fov::toggled = get_value(data, "toggled", vars::set_fov::toggled);
        vars::set_fov::unlock_zoom = get_value(data, "unlock_zoom", vars::set_fov::unlock_zoom);

        // Expander
        vars::combat::hitbox_expander = get_value(data, "hitbox_expander", vars::combat::hitbox_expander);
        vars::combat::hitbox_size_x = get_value(data, "hitbox_size_x", vars::combat::hitbox_size_x);
        vars::combat::hitbox_size_y = get_value(data, "hitbox_size_y", vars::combat::hitbox_size_y);
        vars::combat::hitbox_size_z = get_value(data, "hitbox_size_z", vars::combat::hitbox_size_z);
        vars::combat::hitbox_multiplier = get_value(data, "hitbox_multiplier", vars::combat::hitbox_multiplier);
        vars::combat::hitbox_visible = get_value(data, "hitbox_visible", vars::combat::hitbox_visible);
        vars::combat::hitbox_can_collide = get_value(data, "hitbox_can_collide", vars::combat::hitbox_can_collide);
        vars::combat::hitbox_skip_teammates = get_value(data, "hitbox_skip_teammates", vars::combat::hitbox_skip_teammates);


        std::cout << "Config loaded from " << filename << std::endl;
    }
    else
    {
        std::cerr << "Error: Could not open file for loading config: " << filename << std::endl;
    }
}