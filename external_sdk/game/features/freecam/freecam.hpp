#pragma once
#include "../game/core.hpp"

class c_freecam {
public:
    void run(float dt);

    bool enabled = false;

private:
    uintptr_t original_subject = 0;
    bool rotating = false;
};

inline c_freecam freecam;