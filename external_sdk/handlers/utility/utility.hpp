#pragma once

#include "../../main.hpp"

class c_utility
{
public:
	int get_refresh_rate( );
	void m_print( const char* text, ... );
};

inline c_utility util;