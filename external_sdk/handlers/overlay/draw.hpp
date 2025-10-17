#pragma once
#include "../../main.hpp"

struct ImVec2;
struct ImColor;
struct ImDrawList;

class c_draw
{
public:
    void rectangle( ImVec2 pos, ImVec2 size, ImColor color );
    void outlined_rectangle( ImVec2 top_left, ImVec2 bottom_right, ImColor box_color, ImColor outline_color, float thickness );
    void string( ImVec2 pos, const char* text, ImColor color, bool centered );
    void outlined_string( ImVec2 pos, const char* text, ImColor text_color, ImColor outline_color, bool centered );
    void line( ImVec2 start, ImVec2 end, ImColor color, float thickness );
} ;

inline c_draw draw;