#pragma once
#include "freecam/freecam.hpp"
#include "jump_power/jump_power.hpp"

class c_feature_handler
{
public:
    void start( uintptr_t datamodel );
};

inline c_feature_handler feature_handler;