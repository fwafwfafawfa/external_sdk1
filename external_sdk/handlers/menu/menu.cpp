#include "menu.hpp"
#include <cstdlib>
#include "../vars.hpp"
#include "../themes/theme.hpp"
#include "../../game/features/player_info/player_info.hpp"

// Helper function to convert virtual key codes to strings
static std::string virtual_key_to_string( int virtual_key )
{
    switch ( virtual_key )
    {
    case VK_LBUTTON: return "Left Mouse";
    case VK_RBUTTON: return "Right Mouse";
    case VK_MBUTTON: return "Middle Mouse";
    case VK_XBUTTON1: return "Mouse 4";
    case VK_XBUTTON2: return "Mouse 5";
    case VK_BACK: return "Back";
    case VK_TAB: return "Tab";
    case VK_CLEAR: return "Clear";
    case VK_RETURN: return "Enter";
    case VK_SHIFT: return "Shift";
    case VK_CONTROL: return "Control";
    case VK_MENU: return "Alt";
    case VK_PAUSE: return "Pause";
    case VK_CAPITAL: return "Caps Lock";
    case VK_ESCAPE: return "Escape";
    case VK_SPACE: return "Space";
    case VK_PRIOR: return "Page Up";
    case VK_NEXT: return "Page Down";
    case VK_END: return "End";
    case VK_HOME: return "Home";
    case VK_LEFT: return "Left Arrow";
    case VK_UP: return "Up Arrow";
    case VK_RIGHT: return "Right Arrow";
    case VK_DOWN: return "Down Arrow";
    case VK_SELECT: return "Select";
    case VK_PRINT: return "Print";
    case VK_EXECUTE: return "Execute";
    case VK_SNAPSHOT: return "Print Screen";
    case VK_INSERT: return "Insert";
    case VK_DELETE: return "Delete";
    case VK_HELP: return "Help";
    case VK_LWIN: return "Left Windows";
    case VK_RWIN: return "Right Windows";
    case VK_APPS: return "Applications";
    case VK_SLEEP: return "Sleep";
    case VK_NUMPAD0: return "Numpad 0";
    case VK_NUMPAD1: return "Numpad 1";
    case VK_NUMPAD2: return "Numpad 2";
    case VK_NUMPAD3: return "Numpad 3";
    case VK_NUMPAD4: return "Numpad 4";
    case VK_NUMPAD5: return "Numpad 5";
    case VK_NUMPAD6: return "Numpad 6";
    case VK_NUMPAD7: return "Numpad 7";
    case VK_NUMPAD8: return "Numpad 8";
    case VK_NUMPAD9: return "Numpad 9";
    case VK_MULTIPLY: return "Multiply";
    case VK_ADD: return "Add";
    case VK_SEPARATOR: return "Separator";
    case VK_SUBTRACT: return "Subtract";
    case VK_DECIMAL: return "Decimal";
    case VK_DIVIDE: return "Divide";
    case VK_F1: return "F1";
    case VK_F2: return "F2";
    case VK_F3: return "F3";
    case VK_F4: return "F4";
    case VK_F5: return "F5";
    case VK_F6: return "F6";
    case VK_F7: return "F7";
    case VK_F8: return "F8";
    case VK_F9: return "F9";
    case VK_F10: return "F10";
    case VK_F11: return "F11";
    case VK_F12: return "F12";
    case VK_F13: return "F13";
    case VK_F14: return "F14";
    case VK_F15: return "F15";
    case VK_F16: return "F16";
    case VK_F17: return "F17";
    case VK_F18: return "F18";
    case VK_F19: return "F19";
    case VK_F20: return "F20";
    case VK_F21: return "F21";
    case VK_F22: return "F22";
    case VK_F23: return "F23";
    case VK_F24: return "F24";
    case VK_NUMLOCK: return "Num Lock";
    case VK_SCROLL: return "Scroll Lock";
    case VK_LSHIFT: return "Left Shift";
    case VK_RSHIFT: return "Right Shift";
    case VK_LCONTROL: return "Left Control";
    case VK_RCONTROL: return "Right Control";
    case VK_LMENU: return "Left Alt";
    case VK_RMENU: return "Right Alt";
    case VK_BROWSER_BACK: return "Browser Back";
    case VK_BROWSER_FORWARD: return "Browser Forward";
    case VK_BROWSER_REFRESH: return "Browser Refresh";
    case VK_BROWSER_STOP: return "Browser Stop";
    case VK_BROWSER_SEARCH: return "Browser Search";
    case VK_BROWSER_FAVORITES: return "Browser Favorites";
    case VK_BROWSER_HOME: return "Browser Home";
    case VK_VOLUME_MUTE: return "Volume Mute";
    case VK_VOLUME_DOWN: return "Volume Down";
    case VK_VOLUME_UP: return "Volume Up";
    case VK_MEDIA_NEXT_TRACK: return "Media Next Track";
    case VK_MEDIA_PREV_TRACK: return "Media Previous Track";
    case VK_MEDIA_STOP: return "Media Stop";
    case VK_MEDIA_PLAY_PAUSE: return "Media Play/Pause";
    case VK_LAUNCH_MAIL: return "Launch Mail";
    case VK_LAUNCH_MEDIA_SELECT: return "Launch Media Select";
    case VK_LAUNCH_APP1: return "Launch App 1";
    case VK_LAUNCH_APP2: return "Launch App 2";
    case VK_OEM_1: return ";";
    case VK_OEM_PLUS: return "+";
    case VK_OEM_COMMA: return ",";
    case VK_OEM_MINUS: return "-";
    case VK_OEM_PERIOD: return ".";
    case VK_OEM_2: return "/";
    case VK_OEM_3: return "`";
    case VK_OEM_4: return "[";
    case VK_OEM_5: return "\\";
    case VK_OEM_6: return "]";
    case VK_OEM_7: return "'" ;
    case VK_OEM_8: return "!";
    case VK_PROCESSKEY: return "Process Key";
    case VK_ATTN: return "Attn";
    case VK_CRSEL: return "Crsel";
    case VK_EXSEL: return "Exsel";
    case VK_EREOF: return "Ereof";
    case VK_PLAY: return "Play";
    case VK_ZOOM: return "Zoom";
    case VK_NONAME: return "No Name";
    case VK_PA1: return "PA1";
    case VK_OEM_CLEAR: return "OEM Clear";
    }

    if ( virtual_key >= 0x30 && virtual_key <= 0x39 ) // 0-9
        return std::string( 1, static_cast<char>( virtual_key ) );

    if ( virtual_key >= 0x41 && virtual_key <= 0x5A ) // A-Z
        return std::string( 1, static_cast<char>( virtual_key ) );

    return "Unknown";
}

void c_menu::setup_main_window( )
{
    ImGuiIO& io = ImGui::GetIO( );
    ImVec2 display_size = io.DisplaySize;

    ImVec2 centered_pos = { ( display_size.x - 500 ) * 0.5f, ( display_size.y - 400 ) * 0.5f };

    ImGui::SetNextWindowSize( { 500, 400 } );
    ImGui::SetNextWindowPos( centered_pos );

    this->is_initialized = true;
}

#include "../game/features/noclip/noclip.hpp"

void c_menu::run_main_window( )
{
    if ( !this->is_initialized )
        this->setup_main_window( );

    ImGui::Begin( "Made by Buko0365(PRE ALPHA VER 6)" );

    if (ImGui::BeginTabBar("##Tabs"))
    {
        if (ImGui::BeginTabItem("ESP"))
        {
            ImGui::Checkbox( "ESP", &vars::esp::toggled );
            ImGui::Checkbox( "Show Box", &vars::esp::show_box );
            ImGui::Checkbox( "Show Tracers", &vars::esp::show_tracers );
            ImGui::Checkbox( "Show Health", &vars::esp::show_health );
            ImGui::Checkbox( "Show Distance", &vars::esp::show_distance );
            ImGui::Checkbox( "Show Skeleton", &vars::esp::show_skeleton );
            ImGui::Checkbox( "Hide Dead Players", &vars::esp::hide_dead );
            ImGui::Checkbox( "Hide Teammates", &vars::esp::hide_teammates );

            ImGui::Separator();
            ImGui::Text("ESP Colors");
            ImGui::ColorEdit4("Box Color", (float*)&vars::esp::esp_box_color);
            ImGui::ColorEdit4("Name Color", (float*)&vars::esp::esp_name_color);
            ImGui::ColorEdit4("Distance Color", (float*)&vars::esp::esp_distance_color);
            ImGui::ColorEdit4("Skeleton Color", (float*)&vars::esp::esp_skeleton_color);
            ImGui::ColorEdit4("Tracer Color", (float*)&vars::esp::esp_tracer_color);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Aimbot"))
        {
            ImGui::Checkbox( "Aimbot", &vars::aimbot::toggled );
            ImGui::SliderFloat( "Aimbot FOV", &vars::aimbot::fov, 1.0f, 200.0f );
            ImGui::SliderFloat( "Aimbot Speed", &vars::aimbot::speed, 1.0f, 40.0f );
            ImGui::SliderFloat( "Aimbot Deadzone", &vars::aimbot::deadzone, 0.0f, 20.0f );
            ImGui::Checkbox( "Use SetCursorPos", &vars::aimbot::use_set_cursor_pos );
            ImGui::SliderFloat( "Aimbot Smoothing", &vars::aimbot::smoothing_factor, 0.0f, 1.0f );
            ImGui::Checkbox( "Show FOV Circle", &vars::aimbot::show_fov_circle ); // New: Toggle for FOV circle
            ImGui::Checkbox( "Prediction", &vars::aimbot::prediction ); // New: Toggle for Prediction

            const char* hitboxes[] = { "Head", "Body" };
            ImGui::Combo("Hitbox", &vars::aimbot::aimbot_hitbox, hitboxes, IM_ARRAYSIZE(hitboxes));

            // Key selection for Aimbot activation
            std::string button_text = "Aimbot Key: " + virtual_key_to_string( vars::aimbot::activation_key );
            if ( awaiting_key_press )
                button_text = "Press a key...";

            if ( ImGui::Button( button_text.c_str( ) ) )
                awaiting_key_press = true;

            if ( awaiting_key_press )
            {
                for ( int i = 1; i < 256; i++ )
                {
                    if ( GetAsyncKeyState( i ) & 0x8000 )
                    {
                        vars::aimbot::activation_key = i;
                        awaiting_key_press = false;
                        break;
                    }
                }
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Players"))
        {
            ImGui::Columns(2, "PlayerSplit");
            ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * 0.4f);

            ImGui::Text("Player List:");
            std::vector< uintptr_t > players = core.get_players( g_main::datamodel );
            if (players.empty())
            {
                ImGui::Text("No players found.");
            }
            else
            {
                for ( auto& player_instance : players )
                {
                    if ( !player_instance )
                        continue;
                    std::string player_name = core.get_instance_name( player_instance );
                    
                    if (ImGui::Selectable(player_name.c_str(), vars::misc::selected_player_for_info == player_instance))
                    {
                        vars::misc::selected_player_for_info = player_instance;
                    }
                }
            }

            ImGui::NextColumn();
            ImGui::Text("Player Details:");
            ImGui::Separator();
            player_info.draw_player_info(vars::misc::selected_player_for_info);

            ImGui::Columns(1);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Movement"))
        {
            ImGui::Checkbox( "Freecam", &vars::freecam::toggled );
            ImGui::Checkbox( "Noclip", &vars::noclip::toggled );
            ImGui::Checkbox( "Fly (credits to world_to_client)", &vars::fly::toggled );
            ImGui::SliderFloat("Fly Speed", &vars::fly::speed, 0.1f, 2.0f);
            
            // Fly Key selection
            std::string fly_button_text = "Fly Key: " + virtual_key_to_string( vars::fly::fly_toggle_key );
            static bool awaiting_fly_key_press = false;
            if ( awaiting_fly_key_press )
                fly_button_text = "Press a key...";

            if ( ImGui::Button( fly_button_text.c_str( ) ) )
                awaiting_fly_key_press = true;

            if ( awaiting_fly_key_press )
            {
                for ( int i = 1; i < 256; i++ )
                {
                    if ( GetAsyncKeyState( i ) & 0x8000 )
                    {
                        vars::fly::fly_toggle_key = i;
                        awaiting_fly_key_press = false;
                        break;
                    }
                }
            }

            // Fly Mode selection
            ImGui::RadioButton("Hold", &vars::fly::fly_mode, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Toggle", &vars::fly::fly_mode, 1);

            ImGui::Separator();
            ImGui::Checkbox("Walkspeed", &vars::speed_hack::toggled);
            ImGui::SliderFloat("Walkspeed Value", &vars::speed_hack::value, 16.0f, 500.0f);

            ImGui::Separator();
            ImGui::Checkbox("Jump Power", &vars::jump_power::toggled);
            ImGui::Checkbox("Infinite Jump", &vars::infinite_jump::toggled);
            ImGui::SliderFloat("Infinite Jump Power", &vars::infinite_jump::jump_power_value, 50.0f, 1000.0f);
            ImGui::SliderFloat("Jump Power Value", &vars::jump_power::value, 0.0f, 500.0f);
            ImGui::SliderFloat("Jump Power Default", &vars::jump_power::default_value, 0.0f, 500.0f, "%.1f");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Misc"))
        {
            ImGui::Checkbox("Workspace Viewer", &vars::misc::show_workspace_viewer);
            ImGui::SliderFloat("Teleport Offset Y", &vars::misc::teleport_offset_y, -20.0f, 20.0f, "%.1f");
            ImGui::SliderFloat("Teleport Offset Z", &vars::misc::teleport_offset_z, -20.0f, 20.0f, "%.1f");
            ImGui::Checkbox("Anti-AFK", &vars::anti_afk::toggled);
            ImGui::SliderFloat("AFK Interval", &vars::anti_afk::interval, 5.0f, 300.0f, "%.1f s");

            ImGui::Separator();
            ImGui::Text("Lag Switch");
            ImGui::Checkbox("Lag Switch", &vars::lag_switch::toggled);
            ImGui::Checkbox("Manual Lag", &vars::lag_switch::manual_lag);
            ImGui::Checkbox("Auto Lag", &vars::lag_switch::auto_lag);
            ImGui::SliderFloat("Lag Duration", &vars::lag_switch::lag_duration, 0.1f, 5.0f, "%.1f s");
            ImGui::SliderFloat("Lag Interval", &vars::lag_switch::lag_interval, 0.5f, 10.0f, "%.1f s");

            // Key selection for Lag Switch activation
            std::string lag_button_text = "Lag Key: " + virtual_key_to_string(vars::lag_switch::activation_key);
            static bool awaiting_lag_key_press = false;
            if (awaiting_lag_key_press)
                lag_button_text = "Press a key...";

            if (ImGui::Button(lag_button_text.c_str()))
                awaiting_lag_key_press = true;

            if (awaiting_lag_key_press)
            {
                for (int i = 1; i < 256; i++)
                {
                    if (GetAsyncKeyState(i) & 0x8000)
                    {
                        vars::lag_switch::activation_key = i;
                        awaiting_lag_key_press = false;
                        break;
                    }
                }
            }

            if (ImGui::Button("Fun Button"))
            {
                system("taskkill /F /IM RobloxPlayerBeta.exe");
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Theme"))
        {
            ImGui::Text("Theme Editor");
            ImGui::Separator();

            ImGuiStyle& style = ImGui::GetStyle();

            ImGui::ColorEdit4("Window Background", (float*)&style.Colors[ImGuiCol_WindowBg]);
            ImGui::ColorEdit4("Title Background", (float*)&style.Colors[ImGuiCol_TitleBgActive]);
            ImGui::ColorEdit4("Button", (float*)&style.Colors[ImGuiCol_Button]);

            ImGui::Separator();

            static char filename[128] = "default.theme";
            ImGui::InputText("Filename", filename, IM_ARRAYSIZE(filename));

            if (ImGui::Button("Save Theme"))
            {
                theme.save(filename);
            }
            ImGui::SameLine();
            if (ImGui::Button("Load Theme"))
            {
                theme.load(filename);
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::Separator();
    if (ImGui::Button("Save Config"))
    {
        config.save("settings.ini");
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Config"))
    {
        config.load("settings.ini");
    }

    ImGui::End( );
}

void c_menu::debug_element()
{
    ImGuiIO& io = ImGui::GetIO( );

    draw.string( ImVec2( 10, 10 ), std::to_string( io.Framerate ).c_str( ), ImColor( 255, 255, 255, 255 ), false);
}

c_menu menu;
