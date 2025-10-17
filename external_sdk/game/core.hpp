#pragma once
#include "../main.hpp"

struct vector2d {
    float x, y;

    vector2d ( ) : x ( 0 ), y ( 0 ) {}
    vector2d ( float x, float y ) : x ( x ), y ( y ) {}

    vector2d operator+(const vector2d& other) const { return vector2d(x + other.x, y + other.y); }
    vector2d operator-(const vector2d& other) const { return vector2d(x - other.x, y - other.y); }
    vector2d operator*(float scalar) const { return vector2d(x * scalar, y * scalar); }
};

struct matrix {
    float m [ 4 ] [ 4 ];

    matrix ( ) {
        for ( int i = 0; i < 4; i++ )
            for ( int j = 0; j < 4; j++ )
                m [ i ] [ j ] = 0.0f;
    }

    matrix ( float values [ 4 ] [ 4 ] ) {
        for ( int i = 0; i < 4; i++ )
            for ( int j = 0; j < 4; j++ )
                m [ i ] [ j ] = values [ i ] [ j ];
    }
};

class c_core {
public:
    std::string read_string ( uintptr_t address );
    std::string length_read_string ( uintptr_t string );
    std::string get_instance_name ( uintptr_t instance_address );
    std::string get_instance_classname ( uintptr_t instance_address );
    uintptr_t find_first_child ( uintptr_t instance_address, const std::string &child_name );
    uintptr_t find_first_child_class ( uintptr_t instance_address, const std::string &child_class );
    uintptr_t get_model_instance ( uintptr_t instance_address );
    std::vector < uintptr_t > children ( uintptr_t instance_address );
    std::vector < uintptr_t > get_players ( uintptr_t datamodel_address );
    uintptr_t get_local_humanoid();
    matrix cframe_to_matrix(const CFrame& cframe); // New method
    bool world_to_screen( const vector& world_pos, vector2d& screen_pos, const matrix& view_matrix );
    int get_screen_width( ) { if ( !screen_width ) screen_width = GetSystemMetrics( SM_CXSCREEN ); return screen_width; }
    int get_screen_height( ) { if ( !screen_height ) screen_height = GetSystemMetrics( SM_CYSCREEN ); return screen_height; }
private:
    int screen_width = NULL;
    int screen_height = NULL;
};

inline c_core core;
