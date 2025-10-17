#include "utility.hpp"

// This function is not related to roblox, this will say unexplained.
void c_utility::m_print( const char* text, ... )
{
    va_list args;
    va_start( args, text );
    printf( "\033[94m[info] \033[0m" );
    vprintf( text, args );
    printf( "\n" );
    va_end( args );
}

int c_utility::get_refresh_rate( )
{
    DEVMODE dev_mode = { };
    dev_mode.dmSize = sizeof( dev_mode );

    if ( EnumDisplaySettings( nullptr, ENUM_CURRENT_SETTINGS, &dev_mode ) )
    {
        return dev_mode.dmDisplayFrequency;
    }

    return 60;
}