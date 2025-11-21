#pragma once
#include "../main.hpp"
#include <unordered_map>

class c_noclip {
private:
    std::unordered_map<uintptr_t, bool> original_collision_states;
public:
    void run();
};

inline c_noclip noclip;