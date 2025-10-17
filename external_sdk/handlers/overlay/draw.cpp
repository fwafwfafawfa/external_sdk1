#include "draw.hpp"

void c_draw::rectangle( ImVec2 pos, ImVec2 size, ImColor color )
{
    auto foreground_draw_list = ImGui::GetForegroundDrawList( );

    foreground_draw_list->AddRectFilled( pos, ImVec2( pos.x + size.x, pos.y + size.y ), color );
}

void c_draw::outlined_rectangle( ImVec2 top_left, ImVec2 bottom_right, ImColor box_color, ImColor outline_color, float thickness = 1.0f )
{
    auto foreground_draw_list = ImGui::GetForegroundDrawList( );

    foreground_draw_list->AddRect( ImVec2(top_left.x - 1, top_left.y - 1), ImVec2(bottom_right.x + 1, bottom_right.y + 1), outline_color, 0.0f, 0, thickness + 1.5f );
    foreground_draw_list->AddRect( ImVec2(top_left.x + 1, top_left.y + 1), ImVec2(bottom_right.x - 1, bottom_right.y - 1), outline_color, 0.0f, 0, thickness + 1.5f );

    foreground_draw_list->AddRect( top_left, bottom_right, box_color, 0.0f, 0, thickness );
}

void c_draw::string( ImVec2 pos, const char* text, ImColor color, bool centered = false )
{
    auto foreground_draw_list = ImGui::GetForegroundDrawList( );

    if ( centered )
    {
        ImVec2 text_size = ImGui::CalcTextSize( text );
        pos.x -= text_size.x * 0.5f;
        pos.y -= text_size.y * 0.5f;
    }
    
    foreground_draw_list->AddText( pos, color, text );
}

void c_draw::outlined_string( ImVec2 pos, const char* text, ImColor text_color, ImColor outline_color, bool centered = false )
{
    auto foreground_draw_list = ImGui::GetForegroundDrawList( );

    if ( centered )
    {
        ImVec2 text_size = ImGui::CalcTextSize( text );
        pos.x -= text_size.x * 0.5f;
        pos.y -= text_size.y * 0.5f;
    }
    
    foreground_draw_list->AddText( ImVec2 ( pos.x - 1, pos.y ), outline_color, text );
    foreground_draw_list->AddText( ImVec2 ( pos.x + 1, pos.y ), outline_color, text );
    foreground_draw_list->AddText( ImVec2 ( pos.x, pos.y - 1 ), outline_color, text );
    foreground_draw_list->AddText( ImVec2 ( pos.x, pos.y + 1 ), outline_color, text );
    
    foreground_draw_list->AddText( pos, text_color, text );
}

void c_draw::line( ImVec2 start, ImVec2 end, ImColor color, float thickness = 1.0f )
{
    auto foreground_draw_list = ImGui::GetForegroundDrawList( );
    foreground_draw_list->AddLine( start, end, color, thickness );
}