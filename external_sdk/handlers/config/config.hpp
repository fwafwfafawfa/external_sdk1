#pragma once

#include "../../main.hpp"
#include "../vars.hpp"
#include <string>
#include <fstream>
#include <vector>
#include <map>

class c_config
{
public:
    void save( const std::string& filename );
    void load( const std::string& filename );

private:
    // Helper to convert ImColor to a string (RGBA)
    std::string color_to_string( const ImColor& color );
    // Helper to convert string to ImColor (RGBA)
    ImColor string_to_color( const std::string& str );

    // Helper to get a value from a map
    template<typename T>
    T get_value( const std::map<std::string, std::string>& data, const std::string& key, T default_value );
};

inline c_config config;
