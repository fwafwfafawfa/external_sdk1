#pragma once
#include "../main.hpp"

class c_noclip {
public:
    void run();
    bool enabled = false;
};

inline c_noclip noclip;