#include "esp.hpp"
#include "../game/core.hpp"
#include "../addons/kernel/driver.hpp"
#include "../game/offsets/offsets.hpp"
#include "../handlers/overlay/draw.hpp"
#include "../addons/imgui/imgui.h"
#include "../handlers/utility/utility.hpp" // Include utility.hpp for util.m_print
#include "../../../handlers/vars.hpp" // Include vars.hpp for vars::esp members

void c_esp::run_players( matrix viewmatrix )
{
    std::vector< uintptr_t > players = core.get_players( g_main::datamodel );

    for ( auto& player : players )
    {
        if ( !player )
            continue;

        if ( player == g_main::localplayer )
            continue;

        auto model = core.get_model_instance( player );
        if ( !model )
            continue;

        auto humanoid = core.find_first_child_class( model, "Humanoid" );
        if ( !humanoid )
            continue;

        float health = driver.read< float >( humanoid + offsets::Health );
        float max_health = driver.read< float >( humanoid + offsets::MaxHealth );

        if ( !health )
            continue;

        if ( !max_health )
            continue;

        auto player_root = core.find_first_child( model, "HumanoidRootPart" );
        if ( !player_root )
            continue;

        auto p_player_root = driver.read< uintptr_t >( player_root + offsets::Primitive );
        if ( !p_player_root )
            continue;

        auto player_head = core.find_first_child( model, "Head" );
        if ( !player_head )
            continue;

        auto p_player_head = driver.read< uintptr_t >( player_head + offsets::Primitive );
        if ( !p_player_head )
            continue;

        std::string player_name = core.get_instance_name( player );
        if ( !player_name.c_str( ) )
            continue;

        vector w_player_root = driver.read< vector >( p_player_root + offsets::Position );
        vector w_player_head = driver.read< vector >( p_player_head + offsets::Position );

        vector2d s_player_root;
        if ( !core.world_to_screen( w_player_root, s_player_root, viewmatrix ) )
            continue;

        vector2d s_player_head;
        if ( !core.world_to_screen( vector( w_player_head.x, w_player_head.y + 2.0f, w_player_head.z ), s_player_head, viewmatrix ) )
            continue;

        float height = s_player_root.y - s_player_head.y;
        float width = height * 0.5f;

        ImVec2 top_left = ImVec2( s_player_root.x - ( width * 0.5f ), s_player_head.y );
        ImVec2 bottom_right = ImVec2( s_player_root.x + ( width * 0.5f ), s_player_root.y );

        draw.outlined_rectangle( top_left, bottom_right, vars::esp::esp_box_color, ImColor( 0, 0, 0, 255 ), 1.0f );

        float health_percent = health / max_health;
        health_percent = min( max( health_percent, 0.0f ), 1.0f );

        ImColor health_color = ImColor(
            static_cast< int >( 255 * ( 1.0f - health_percent ) ),
            static_cast< int >( 255 * health_percent ),
            0,
            255
        );

        draw.outlined_string( ImVec2( s_player_root.x, top_left.y - 15 ), player_name.c_str( ), vars::esp::esp_name_color, ImColor( 0, 0, 0, 255 ), true );
        
        if (vars::esp::show_health)
        {
            std::stringstream ss_health;
            ss_health << "[HP: " << std::fixed << std::setprecision(0) << health << "]";
            std::string health_str = ss_health.str();
            draw.outlined_string( ImVec2( s_player_root.x, s_player_root.y + 5 ), health_str.c_str(), health_color, ImColor(0, 0, 0, 255), true );
        }

        // --- Distance ESP ---
        if (vars::esp::show_distance)
        {
            // Get local player's character model
            uintptr_t local_player_character_model = core.find_first_child(core.find_first_child_class(g_main::datamodel, "Workspace"), core.get_instance_name(g_main::localplayer));
            if (local_player_character_model)
            {
                // Get local player's HumanoidRootPart
                auto local_player_root = core.find_first_child(local_player_character_model, "HumanoidRootPart");
                if (local_player_root)
                {
                    auto p_local_player_root = driver.read< uintptr_t >( local_player_root + offsets::Primitive );
                    if (p_local_player_root)
                    {
                        vector w_local_player_root = driver.read< vector >( p_local_player_root + offsets::Position );

                        // Calculate distance to target player's HumanoidRootPart
                        float distance = sqrtf(
                            powf(w_player_root.x - w_local_player_root.x, 2) +
                            powf(w_player_root.y - w_local_player_root.y, 2) +
                            powf(w_player_root.z - w_local_player_root.z, 2)
                        );

                        std::stringstream ss_distance;
                        ss_distance << "[" << std::fixed << std::setprecision(0) << distance << "m]";
                        std::string distance_str = ss_distance.str();
                        draw.outlined_string( ImVec2( s_player_root.x, s_player_root.y + 20 ), distance_str.c_str(), vars::esp::esp_distance_color, ImColor( 0, 0, 0, 255 ), true );
                    }
                }
            }
        }
        // --- End Distance ESP ---

        int screen_width = core.get_screen_width( );
        int screen_height = core.get_screen_height( );
        draw.line( ImVec2( screen_width * 0.5f, screen_height ), ImVec2( s_player_root.x, s_player_root.y ), ImColor( 255, 255, 255, 255 ), 1.0f );

        // --- Skeleton ESP ---
        if (vars::esp::show_skeleton)
        {
            // Define a simplified skeleton structure (bone name, parent bone name)
            // This assumes common Roblox character part names
            struct BoneInfo { const char* name; const char* parent_name; };
            BoneInfo skeleton_bones[] = {
                // Torso to Head/Arms/Legs
                {"Head", "UpperTorso"},
                {"UpperTorso", "HumanoidRootPart"},
                {"LowerTorso", "HumanoidRootPart"}, // Connect LowerTorso to Root
                {"UpperTorso", "LowerTorso"},       // Connect UpperTorso to LowerTorso

                // Arms
                {"RightUpperArm", "UpperTorso"},
                {"RightLowerArm", "RightUpperArm"},
                {"RightHand", "RightLowerArm"},

                {"LeftUpperArm", "UpperTorso"},
                {"LeftLowerArm", "LeftUpperArm"},
                {"LeftHand", "LeftLowerArm"},

                // Legs
                {"RightUpperLeg", "LowerTorso"},
                {"RightLowerLeg", "RightUpperLeg"},
                {"RightFoot", "RightLowerLeg"},

                {"LeftUpperLeg", "LowerTorso"},
                {"LeftLowerLeg", "LeftUpperLeg"},
                {"LeftFoot", "LeftLowerLeg"}
            };

            std::unordered_map<std::string, vector> bone_world_positions;
            std::unordered_map<std::string, vector2d> bone_screen_positions;

            // Get positions of all relevant bones
            for (const auto& bone_info : skeleton_bones)
            {
                uintptr_t bone_part = core.find_first_child(model, bone_info.name);
                if (bone_part)
                {
                    uintptr_t p_bone_part = driver.read<uintptr_t>(bone_part + offsets::Primitive);
                    if (p_bone_part)
                    {
                        vector w_bone_pos = driver.read<vector>(p_bone_part + offsets::Position);
                        bone_world_positions[bone_info.name] = w_bone_pos;

                        vector2d s_bone_pos;
                        if (core.world_to_screen(w_bone_pos, s_bone_pos, viewmatrix))
                        {
                            bone_screen_positions[bone_info.name] = s_bone_pos;
                        }
                    }
                }
            }

            // Draw lines between connected bones
            for (const auto& bone_info : skeleton_bones)
            {
                if (bone_screen_positions.count(bone_info.name) && bone_screen_positions.count(bone_info.parent_name))
                {
                    draw.line(
                        ImVec2(bone_screen_positions[bone_info.name].x, bone_screen_positions[bone_info.name].y),
                        ImVec2(bone_screen_positions[bone_info.parent_name].x, bone_screen_positions[bone_info.parent_name].y),
                        vars::esp::esp_skeleton_color, // Configurable skeleton color
                        1.0f
                    );
                }
            }
        }
        // --- End Skeleton ESP ---
    }
}

void c_esp::run_aimbot( matrix viewmatrix )
{
    uintptr_t closest_player = 0;
    float closest_distance = FLT_MAX;

    vector2d crosshair_pos = { static_cast<float>(core.get_screen_width( ) / 2), static_cast<float>(core.get_screen_height( ) / 2) };

    // Draw FOV Circle
    if (vars::aimbot::show_fov_circle)
    {
        draw.circle(ImVec2(crosshair_pos.x, crosshair_pos.y), vars::aimbot::fov, ImColor(255, 255, 255, 255), 1.0f);
    }

    if ( !vars::aimbot::toggled || !GetAsyncKeyState( vars::aimbot::activation_key ) )
    {
        this->leftover_x = 0.0f;
        this->leftover_y = 0.0f;
        this->smoothed_delta_x = 0.0f;
        this->smoothed_delta_y = 0.0f;
        return;
    }

    std::vector< uintptr_t > players = core.get_players( g_main::datamodel );

    for ( auto& player : players )
    {
        if ( !player || player == g_main::localplayer )
            continue;

        uintptr_t player_team = driver.read<uintptr_t>( player + offsets::Team );
        if ( player_team == g_main::localplayer_team && player_team != 0 )
            continue;

        auto model = core.get_model_instance( player );
        if ( !model )
            continue;

        auto humanoid = core.find_first_child_class( model, "Humanoid" );
        if ( !humanoid )
            continue;

        float health = driver.read< float >( humanoid + offsets::Health );
        if ( !health )
            continue;

        // Get HumanoidRootPart for velocity
        auto player_root = core.find_first_child( model, "HumanoidRootPart" );
        if ( !player_root )
            continue;

        auto p_player_root = driver.read< uintptr_t >( player_root + offsets::Primitive );
        if ( !p_player_root )
            continue;

        // Determine target bone based on aimbot_hitbox setting
        const char* target_bone_name = (vars::aimbot::aimbot_hitbox == 0) ? "Head" : "HumanoidRootPart";
        auto target_bone = core.find_first_child( model, target_bone_name );
        if ( !target_bone )
            continue;
        auto p_target_bone = driver.read< uintptr_t >( target_bone + offsets::Primitive );
        if ( !p_target_bone )
            continue;

        vector w_target_bone_pos = driver.read< vector >( p_target_bone + offsets::Position );
        vector v_player_root = driver.read< vector >( p_player_root + offsets::Velocity ); // Read velocity for prediction

        // Apply prediction if enabled
        if (vars::aimbot::prediction) {
            float time_to_target = 0.1f; // Simple fixed prediction time (adjust as needed)
            w_target_bone_pos = w_target_bone_pos + (v_player_root * time_to_target);
        }

        vector2d s_target_bone_pos;

        if ( !core.world_to_screen( w_target_bone_pos, s_target_bone_pos, viewmatrix ) )
            continue;

        float distance = sqrt( pow( s_target_bone_pos.x - crosshair_pos.x, 2 ) + pow( s_target_bone_pos.y - crosshair_pos.y, 2 ) );

        if ( distance < closest_distance && distance < vars::aimbot::fov )
        {
            closest_distance = distance;
            closest_player = player;
        }
    }

    if ( closest_player )
    {
        auto model = core.get_model_instance( closest_player );

        // Get HumanoidRootPart for velocity again for the final aim
        auto player_root = core.find_first_child( model, "HumanoidRootPart" );
        auto p_player_root = driver.read< uintptr_t >( player_root + offsets::Primitive );
        vector v_player_root = driver.read< vector >( p_player_root + offsets::Velocity );

        // Determine target bone based on aimbot_hitbox setting for final aim
        const char* target_bone_name = (vars::aimbot::aimbot_hitbox == 0) ? "Head" : "HumanoidRootPart";
        auto target_bone = core.find_first_child( model, target_bone_name );
        auto p_target_bone = driver.read< uintptr_t >( target_bone + offsets::Primitive );
        vector w_target_bone_pos = driver.read< vector >( p_target_bone + offsets::Position );
        // vector v_player_root = driver.read< vector >( p_player_root + offsets::Velocity ); // Not needed without prediction

        vector2d s_target_bone_pos;

        if ( core.world_to_screen( w_target_bone_pos, s_target_bone_pos, viewmatrix ) )
        {
            float delta_x = s_target_bone_pos.x - crosshair_pos.x;
            float delta_y = s_target_bone_pos.y - crosshair_pos.y;

            // Apply exponential smoothing
            this->smoothed_delta_x = (delta_x * vars::aimbot::smoothing_factor) + (this->smoothed_delta_x * (1.0f - vars::aimbot::smoothing_factor));
            this->smoothed_delta_y = (delta_y * vars::aimbot::smoothing_factor) + (this->smoothed_delta_y * (1.0f - vars::aimbot::smoothing_factor));

            // Apply deadzone
            if ( abs( this->smoothed_delta_x ) < vars::aimbot::deadzone && abs( this->smoothed_delta_y ) < vars::aimbot::deadzone )
            {
                this->leftover_x = 0.0f;
                this->leftover_y = 0.0f;
                return;
            }

            float aim_delta_x = this->smoothed_delta_x / vars::aimbot::speed;
            float aim_delta_y = this->smoothed_delta_y / vars::aimbot::speed;

            if ( vars::aimbot::use_set_cursor_pos )
            {
                // Calculate target absolute position
                int target_x = static_cast<int>(s_target_bone_pos.x);
                int target_y = static_cast<int>(s_target_bone_pos.y);

                // Get current cursor position
                POINT current_mouse_pos;
                GetCursorPos(&current_mouse_pos);

                // Calculate smoothed movement
                float smooth_factor = 1.0f / vars::aimbot::speed; // Use speed as smoothing factor
                int move_x = static_cast<int>(current_mouse_pos.x + (target_x - current_mouse_pos.x) * smooth_factor);
                int move_y = static_cast<int>(current_mouse_pos.y + (target_y - current_mouse_pos.y) * smooth_factor);

                // Ensure at least 1 pixel movement if not at target
                if (move_x == current_mouse_pos.x && target_x != current_mouse_pos.x) {
                    move_x += (target_x > current_mouse_pos.x ? 1 : -1);
                }
                if (move_y == current_mouse_pos.y && target_y != current_mouse_pos.y) {
                    move_y += (target_y > current_mouse_pos.y ? 1 : -1);
                }

                util.m_print("SetCursorPos Debug: Target(%d, %d), Current(%d, %d), Move(%d, %d)", target_x, target_y, current_mouse_pos.x, current_mouse_pos.y, move_x, move_y);

                SetCursorPos(move_x, move_y);

                // Reset leftovers for SendInput if we switch back
                this->leftover_x = 0.0f;
                this->leftover_y = 0.0f;
            }
            else
            {
                // Add leftovers from previous frame
                aim_delta_x += this->leftover_x;
                aim_delta_y += this->leftover_y;

                // Get the integer part for SendInput
                LONG move_x = static_cast<LONG>(aim_delta_x);
                LONG move_y = static_cast<LONG>(aim_delta_y);

                // Store the fractional part for next frame
                this->leftover_x = aim_delta_x - move_x;
                this->leftover_y = aim_delta_y - move_y;

                INPUT input = { 0 };
                input.type = INPUT_MOUSE;
                input.mi.dx = move_x;
                input.mi.dy = move_y;
                input.mi.dwFlags = MOUSEEVENTF_MOVE;

                SendInput(1, &input, sizeof(INPUT));
            }
        }
    }
    else
    {
        // Reset leftovers if no target
        this->leftover_x = 0.0f;
        this->leftover_y = 0.0f;
        this->smoothed_delta_x = 0.0f;
        this->smoothed_delta_y = 0.0f;
    }
}