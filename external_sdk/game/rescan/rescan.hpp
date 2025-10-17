#pragma once
#include "../../main.hpp"

class c_rescan
{
private:
    uintptr_t m_last_place_id = 0;
    std::chrono::steady_clock::time_point m_last_check_time;
    const std::chrono::seconds m_check_interval{ 1 };

public:
    c_rescan( )
    {
        m_last_check_time = std::chrono::steady_clock::now( );
    }

    void start_search( );
};

inline c_rescan rescan;